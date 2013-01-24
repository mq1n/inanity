#include "BufferedFileSystem.hpp"
#include "BufferedInputStream.hpp"
#include "BufferedOutputStream.hpp"
#include "Exception.hpp"

BufferedFileSystem::BufferedFileSystem(ptr<FileSystem> fileSystem)
: FilterFileSystem(fileSystem) {}

ptr<InputStream> BufferedFileSystem::LoadStream(const String& fileName)
{
	try
	{
		return NEW(BufferedInputStream(fileSystem->LoadStream(fileName)));
	}
	catch(Exception* exception)
	{
		THROW_SECONDARY_EXCEPTION("Can't load stream from buffered file system", exception);
	}
}

ptr<OutputStream> BufferedFileSystem::SaveStream(const String& fileName)
{
	try
	{
		return NEW(BufferedOutputStream(fileSystem->SaveStream(fileName)));
	}
	catch(Exception* exception)
	{
		THROW_SECONDARY_EXCEPTION("Can't save stream in buffered file system", exception);
	}
}
