#include "DeflateStream.hpp"
#include "../MemoryFile.hpp"
#include "../MemoryStream.hpp"
#include "../Exception.hpp"
#include <memory.h>
#include <algorithm>

BEGIN_INANITY_DATA

DeflateStream::DeflateStream(ptr<OutputStream> outputStream, CompressionLevel compressionLevel)
: inputFile(NEW(MemoryFile(inputBufferSize))), outputStream(outputStream), finalized(false)
{
	try
	{
		//инициализировать поток zlib
		zstream.zalloc = Z_NULL;
		zstream.zfree = Z_NULL;
		zstream.opaque = Z_NULL;
		switch(deflateInit(&zstream, compressionLevel))
		{
		case Z_OK:
			break;
		case Z_STREAM_ERROR:
			THROW("Invalid compression level");
		default:
			THROW("Can't initialize deflation");
		}

		//выделить память под выходной буфер
		unsigned outputBound = deflateBound(&zstream, inputBufferSize);
		outputFile = NEW(MemoryFile(outputBound));

		//указать параметры буферов для потока
		zstream.next_in = (Bytef*)inputFile->GetData();
		zstream.avail_in = 0;
		zstream.next_out = (Bytef*)outputFile->GetData();
		zstream.avail_out = outputBound;
	}
	catch(Exception* exception)
	{
		THROW_SECONDARY("Can't initialize compress stream", exception);
	}
}

DeflateStream::~DeflateStream()
{
	//освободить ресурсы; мы не сбрасываем текущие данные из буфера,
	//так как это может вызвать исключение
	//деструктор здесь только предупреждает утечку ресурсов
	if(!finalized)
		deflateEnd(&zstream);
}

void DeflateStream::WriteOutput(void* data, size_t size)
{
	//записать данные в выходной поток
	outputStream->Write(data, size);
}

/*
Между вызовами Write и между итерациями цикла внутри Write поддерживается
следующий инвариант.
Входной буфер - может содержать данные
next_in - начало входного буфера
avail_in - размер этих данных
Выходной буфер - всегда пустой
next_out - начало выходного буфера
avail_out - размер выходного буфера
*/
void DeflateStream::Write(const void* data, size_t size)
{
	try
	{
		//если финализация уже была выполнена, то записывать данные нельзя
		if(finalized)
			THROW("Stream already finalized");

		//запомнить параметры буферов
		Bytef* inputBuffer = zstream.next_in;
		Bytef* outputBuffer = zstream.next_out;
		unsigned outputSize = zstream.avail_out;

		//пока есть данные
		while(size)
		{
			//приписать данные к концу входного буфера
			unsigned toRead = (unsigned)std::min<size_t>(size, inputBufferSize - zstream.avail_in);
			memcpy(zstream.next_in + zstream.avail_in, data, toRead);
			zstream.avail_in += toRead;
			data = (char*)data + toRead;
			size -= toRead;
			//если входной буфер переполнен
			if(zstream.avail_in >= inputBufferSize)
			{
				//выполнить сжатие данных
				switch(deflate(&zstream, Z_NO_FLUSH))
				{
				case Z_OK:
				case Z_BUF_ERROR:
					break;
				default:
					THROW("Compression error");
				}

				//записать данные в выходной поток
				WriteOutput(outputBuffer, outputSize - zstream.avail_out);

				//перенести данные, которые остались во входном буфере, в его начало
				memmove(inputBuffer, zstream.next_in, zstream.avail_in);
				zstream.next_in = inputBuffer;
				//очистить выходной буфер
				zstream.next_out = outputBuffer;
				zstream.avail_out = outputSize;
			}
		}
	}
	catch(Exception* exception)
	{
		THROW_SECONDARY("Can't compress data", exception);
	}
}

/*
Во время и после Flush указанный для Write инвариант не выполняется.
*/
void DeflateStream::Flush()
{
	try
	{
		//если финализация уже была выполнена, закончить
		if(finalized) return;

		//запомнить параметры выходного буфера
		Bytef* outputBuffer = zstream.next_out;
		unsigned outputSize = zstream.avail_out;

		//цикл, пока компрессия не завершена
		for(;;)
		{
			//выполнять компрессию
			int result = deflate(&zstream, Z_FINISH);

			//обработка ошибок
			switch(result)
			{
			case Z_STREAM_END:
			case Z_OK:
				break;
			default:
				THROW("Compression error");
			}

			//записать вывод в поток
			WriteOutput(outputBuffer, outputSize - zstream.avail_out);
			//очистить выходной буфер
			zstream.next_out = outputBuffer;
			zstream.avail_out = outputSize;

			//если это конец, закончить
			if(result == Z_STREAM_END)
				break;
		}

		//завершить сжатие
		if(deflateEnd(&zstream) != Z_OK)
			THROW("Finalize stream error");

		//установить флажок завершенности
		finalized = true;
	}
	catch(Exception* exception)
	{
		THROW_SECONDARY("Can't finalize compression", exception);
	}
}

ptr<File> DeflateStream::CompressFile(ptr<File> file, CompressionLevel compressionLevel)
{
	try
	{
		//создать выходной поток
		ptr<MemoryStream> outputStream = NEW(MemoryStream);
		//создать поток для сжатия
		ptr<DeflateStream> stream = NEW(DeflateStream(&*outputStream, compressionLevel));

		//сжать данные
		stream->Write(file->GetData(), file->GetSize());
		stream->Flush();
		//вернуть файл
		return outputStream->ToFile();
	}
	catch(Exception* exception)
	{
		THROW_SECONDARY("Can't decompress to file", exception);
	}
}

ptr<DeflateStream> DeflateStream::CreateMax(ptr<OutputStream> outputStream)
{
	return NEW(DeflateStream(outputStream, compressionMax));
}

END_INANITY_DATA
