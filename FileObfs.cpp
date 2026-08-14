#include "obxrac32b64.hpp"
#include <fstream>
#include <chrono>
struct File {
	void log(std::string data) {
		std::cout << data;
	}
	uint8_t valid(auto& file) {
		return file.is_open();
	}

	void open(std::string sFileName,std::string* sBuf) {
		std::ifstream filein(sFileName, std::ios::binary);
		if (valid(filein)) {
			filein.seekg(0, std::ios::end);
			uint64_t sz = filein.tellg();
			filein.seekg(0, std::ios::beg);
			log("     fs->read() " + sFileName + " opened size: " + std::to_string(sz) + " bytes\n");
			std::string data(sz, '\0');
			filein.read(data.data(), sz);
			if (!data.empty()) {
				*sBuf = data;
				log("     fs->read()  copy to buf \n");
			}
		}
	}
	void write(std::string sFileName, std::string sData) {
		std::ofstream fileout(sFileName, std::ios::binary);
		if (valid(fileout)) {
			uint64_t sz = sData.size();
			log("     fs->write() " + sFileName + " out size: " + std::to_string(sz) + " bytes\n");
			if (sz > 0) {
				fileout.write(sData.c_str(), sz);
			}
			else {
				log("     fs->write() -> data it's empty!\n");
			}
		}
	}
};
uint64_t u64ArgvSize(char** argv,int index) {
	return strlen(reinterpret_cast<const char*>(argv[index]));
}
int main(int argc, char** argv) {
	File* fs = new File();
	if (argc < 3) {
		std::cout << "     usage FileObfs.exe file.mp4 423r32f234d21eq" << argc << "\n";
		std::cout << "                         file           key" << "\n";
		std::cout << "     Error argv! argc=" << argc << "\n";
	}
	else {
		std::cout << "\n\n\n     FileObfs by hcpp20334 =======================================\n";
		std::string sFile = std::string(reinterpret_cast<const char*>((u64ArgvSize(argv, 1) == 0 ? "N/A" : argv[1])));
		std::string sKey  = std::string(reinterpret_cast<const char*>((u64ArgvSize(argv, 2) == 0 ? "N/A" : argv[2])));
		std::string sMode = std::string(reinterpret_cast<const char*>((u64ArgvSize(argv, 3) == 0 ? "N/A" : argv[3])));
		std::string sFileBufferData = "";
		std::string sFileBufferDataOutput = "";
		uint8_t u8Mode = (sMode == "-d" ? 1 : 0);
		std::cout << "     offsets [file= " << sFile << " | key= " << sKey <<" | mode="<<(u8Mode ? " decode ": " encode " )<< " ]\n";
		auto open_time_in = std::chrono::high_resolution_clock::now();
		fs->open(sFile, &sFileBufferData);
		auto open_time_end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> open_time_elapsed = (open_time_end - open_time_in);
		if (!sFileBufferData.empty()) {
			fs->log("     (fs) size=" + std::to_string(sFileBufferData.size()));
			auto obfs_time_in = std::chrono::high_resolution_clock::now();
			sFileBufferDataOutput = obxrac32b64(bool(u8Mode), sFileBufferData, sKey);
			auto obfs_time_end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> obfs_time_elapsed = (obfs_time_end - obfs_time_in);
			if (!sFileBufferDataOutput.empty()) {
				auto write_time_in = std::chrono::high_resolution_clock::now();
				fs->write(sFile + (u8Mode ? ".dec":".fobs"), sFileBufferDataOutput);
				auto write_time_end = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> write_time_elapsed = (write_time_end - write_time_in);
				std::cout << std::format(
					"     fs->open() time elapsed: {}\n     obxrac32b64() time elapsed: {}\n     fs->write() time elapsed: {}\n",
					open_time_elapsed, obfs_time_elapsed, write_time_elapsed);
				fs->write(sFile + ".meta", ("key="+sKey));
			}
		}
	}
	delete fs;
}