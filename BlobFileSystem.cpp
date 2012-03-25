#include "BlobFileSystem.hpp"
#include "PartFile.hpp"
#include "FileInputStream.hpp"
#include "StreamReader.hpp"
#include "Exception.hpp"
#include <string.h>

const unsigned BlobFileSystem::Terminator::signatureValue = 'BOLB';

BlobFileSystem::BlobFileSystem(ptr<File> file) : file(file)
{
	try
	{
		void* fileData = file->GetData();
		size_t fileSize = file->GetSize();
		size_t size = fileSize;

		//получить терминатор
		if(size < sizeof(Terminator))
			THROW_PRIMARY_EXCEPTION("Can't read terminator");
		Terminator* terminator = (Terminator*)((char*)fileData + fileSize) - 1;
		size -= sizeof(*terminator);

		//проверить сигнатуру
		if(terminator->signature != Terminator::signatureValue)
			THROW_PRIMARY_EXCEPTION("Invalid signature");

		//проверить, что заголовок читается
		if(size < terminator->headerSize)
			THROW_PRIMARY_EXCEPTION("Can't read header");

		//получить читатель заголовка
		ptr<StreamReader> headerReader = NEW(StreamReader(NEW(FileInputStream(
			NEW(PartFile(file, (char*)terminator - terminator->headerSize, terminator->headerSize))))));
		size -= terminator->headerSize;

		//считывать файлы, пока есть
		for(;;)
		{
			//считать имя файла
			String fileName = headerReader->ReadString();
			//если оно пустое, значит, это конец
			if(fileName.empty())
				break;

			//считать смещение до файла и его размер
			size_t fileOffset = headerReader->ReadShortly();
			size_t fileSize = headerReader->ReadShortly();

			//проверить, что файл читается
			if(fileOffset > size || size - fileOffset < fileSize)
				THROW_PRIMARY_EXCEPTION("Can't read file " + fileName);
			
			//добавить файл в карту
			files[fileName] = NEW(PartFile(file, (char*)fileData + fileOffset, fileSize));
		}
	}
	catch(Exception* exception)
	{
		THROW_SECONDARY_EXCEPTION("Can't load blob file system", exception);
	}
}

ptr<File> BlobFileSystem::TryLoadFile(const String& fileName)
{
	std::unordered_map<String, ptr<File> >::const_iterator it = files.find(fileName);
	if(it == files.end())
		return 0;
	return it->second;
}

void BlobFileSystem::GetFileNames(std::vector<String>& fileNames) const
{
	for(std::unordered_map<String, ptr<File> >::const_iterator i = files.begin(); i != files.end(); ++i)
		fileNames.push_back(i->first);
}

bool BlobFileSystem::IsFileExists(const String& fileName) const
{
	return files.find(fileName) != files.end();
}
