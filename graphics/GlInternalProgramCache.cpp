#include "GlInternalProgramCache.hpp"
#include "GlVertexShader.hpp"
#include "GlPixelShader.hpp"
#include "GlShaderBindings.hpp"
#include "GlDevice.hpp"
#include "GlSystem.hpp"
#include "../Exception.hpp"
#include "opengl.hpp"

size_t std::hash<Inanity::Graphics::GlInternalProgramKey>::operator()(const Inanity::Graphics::GlInternalProgramKey& key) const
{
	return size_t(key.vertexShader) ^ size_t(key.pixelShader);
}

BEGIN_INANITY_GRAPHICS

bool operator==(const GlInternalProgramKey& a, const GlInternalProgramKey& b)
{
	return a.vertexShader == b.vertexShader && a.pixelShader == b.pixelShader;
}

GlInternalProgramKey::GlInternalProgramKey(GlVertexShader* vertexShader, GlPixelShader* pixelShader)
: vertexShader(vertexShader), pixelShader(pixelShader) {}

GlInternalProgramCache::GlInternalProgramCache(ptr<GlDevice> device)
: device(device) {}

void GlInternalProgramCache::ApplyPreLinkBindings(GLuint programName, ptr<GlShaderBindings> shaderBindings)
{
	// применить привязки атрибутов
	const GlShaderBindings::Bindings& attributeBindings = shaderBindings->GetAttributeBindings();
	for(size_t i = 0; i < attributeBindings.size(); ++i)
	{
		// привязать атрибут к заданному generic vertex attribute index
		glBindAttribLocation(programName, attributeBindings[i].second, attributeBindings[i].first.c_str());
		GlSystem::CheckErrors("Can't bind attribute location");
	}

	// применить привязки выходных переменных пиксельного шейдера
	const GlShaderBindings::Bindings& targetBindings = shaderBindings->GetTargetBindings();
	// dual source color blending
	if(shaderBindings->GetDualFragmentTarget())
	{
		glBindFragDataLocationIndexed(programName, 0, targetBindings[0].second, targetBindings[0].first.c_str());
		glBindFragDataLocationIndexed(programName, 0, targetBindings[1].second, targetBindings[1].first.c_str());
		GlSystem::CheckErrors("Can't bind dual frag data location");
	}
	else
	{
		for(size_t i = 0; i < targetBindings.size(); ++i)
		{
			// привязать переменную к заданному номеру цветового рендертаргета
			glBindFragDataLocation(programName, targetBindings[i].second, targetBindings[i].first.c_str());
			GlSystem::CheckErrors("Can't bind frag data location");
		}
	}
}

void GlInternalProgramCache::ApplyPostLinkBindings(GLuint programName, ptr<GlShaderBindings> shaderBindings)
{
	// применить привязки uniform-блоков
	const GlShaderBindings::Bindings& uniformBlockBindings = shaderBindings->GetUniformBlockBindings();
	for(size_t i = 0; i < uniformBlockBindings.size(); ++i)
	{
		// получить индекс блока
		GLuint index = glGetUniformBlockIndex(programName, uniformBlockBindings[i].first.c_str());
		GlSystem::CheckErrors("Can't get uniform block index");
		// привязать индекс к нужному binding point
		glUniformBlockBinding(programName, index, uniformBlockBindings[i].second);
		GlSystem::CheckErrors("Can't bind uniform block index");
	}

	// применить привязки семплеров
	const GlShaderBindings::Bindings& samplerBindings = shaderBindings->GetSamplerBindings();
	for(size_t i = 0; i < samplerBindings.size(); ++i)
	{
		// получить location семплера
		GLuint location = glGetUniformLocation(programName, samplerBindings[i].first.c_str());
		GlSystem::CheckErrors("Can't get location of sampler");
		// привязать location к нужному texture sampling unit
		glUniform1i(location, samplerBindings[i].second);
		GlSystem::CheckErrors("Can't bind program sampler to texture unit");
	}
}

void GlInternalProgramCache::GetNonBufferUniformBindings(
	GLuint programName,
	ptr<GlShaderBindings> shaderBindings,
	GlInternalProgram::UniformBindings& uniformBindings)
{
	const GlShaderBindings::UniformBindings& shaderUniformBindings = shaderBindings->GetUniformBindings();
	for(size_t i = 0; i < shaderUniformBindings.size(); ++i)
	{
		const GlShaderBindings::UniformBinding& a = shaderUniformBindings[i];
		GlInternalProgram::UniformBinding b;
		b.dataType = a.dataType;
		b.count = a.count;
		b.slot = a.slot;
		b.offset = a.offset;
		b.location = glGetUniformLocation(programName, a.name.c_str());
		GlSystem::CheckErrors("Can't get uniform location");
		uniformBindings.push_back(b);
	}
}

ptr<GlInternalProgram> GlInternalProgramCache::GetProgram(GlVertexShader* vertexShader, GlPixelShader* pixelShader)
{
	GlInternalProgramKey key(vertexShader, pixelShader);
	Programs::const_iterator i = programs.find(key);
	if(i != programs.end())
		return i->second;

	// создать программу
	GLuint programName = glCreateProgram();
	GlSystem::CheckErrors("Can't create program");

	// присоединить шейдеры
	if(vertexShader)
	{
		glAttachShader(programName, vertexShader->GetShaderName());
		GlSystem::CheckErrors("Can't attach vertex shader");
	}
	if(pixelShader)
	{
		glAttachShader(programName, pixelShader->GetShaderName());
		GlSystem::CheckErrors("Can't attach pixel shader");
	}

	// применить pre-link привязки
	ApplyPreLinkBindings(programName, vertexShader->GetShaderBindings());
	ApplyPreLinkBindings(programName, pixelShader->GetShaderBindings());

	// слинковать программу
	glLinkProgram(programName);
	GlSystem::CheckErrors("Can't link program");
	{
		GLint status;
		glGetProgramiv(programName, GL_LINK_STATUS, &status);
		if(status != GL_TRUE)
		{
			// получить лог ошибок
			GLint logLength;
			glGetProgramiv(programName, GL_INFO_LOG_LENGTH, &logLength);
			String log(logLength, ' ');
			glGetProgramInfoLog(programName, logLength, &logLength, &*log.begin());
			log.resize(logLength);

			// очистить ошибки, на всякий случай
			GlSystem::ClearErrors();

			// выбросить ошибку
			THROW("Can't link program:\n" + log);
		}
	}

	// указать программу как текущую
	glUseProgram(programName);
	GlSystem::CheckErrors("Can't bind program");

	// применить post-link привязки
	ApplyPostLinkBindings(programName, vertexShader->GetShaderBindings());
	ApplyPostLinkBindings(programName, pixelShader->GetShaderBindings());

	// get non-buffer uniform bindings
	GlInternalProgram::UniformBindings uniformBindings;
	GetNonBufferUniformBindings(programName, vertexShader->GetShaderBindings(), uniformBindings);
	GetNonBufferUniformBindings(programName, pixelShader->GetShaderBindings(), uniformBindings);

	// отвязать программу
	glUseProgram(0);
	GlSystem::CheckErrors("Can't unbind program");

	// записать программу
	ptr<GlInternalProgram> program = NEW(GlInternalProgram(device, programName, uniformBindings));
	programs[key] = program;

	return program;
}

END_INANITY_GRAPHICS
