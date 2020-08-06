#ifndef ELF_H
#define ELF_H
#include <stdint.h>
#include <iostream>
#include <vector>
#include <typeinfo>

using std::string;
using std::vector;

const char ElfMagic[] = { 0x7f, 'E', 'L', 'F', '\0' };
const int EI_CLASS = 4; // File class
const int EI_NIDENT = 16;
const int EI_DATA = 5; // Data encoding
// Segment types.
const int PT_NULL = 0; // Unused segment.

const int PT_LOAD = 1; // Loadable segment.;

const int PT_DYNAMIC = 2; // Dynamic linking information.
const int PT_INTERP = 3; // Interpreter pathname.
const int PT_NOTE = 4; // Auxiliary information.
const int PT_SHLIB = 5; // Reserved.
const int PT_PHDR = 6; // The program header table itself.
const int PT_TLS = 7; // The thread-local storage template.
// Segment flag bits.
const int PF_X = 1; // Execute
const int PF_W = 2; // Write
const int PF_R = 4; // Read

// Special section indices.
const int SHN_UNDEF = 0; // Undefined, missing, irrelevant, or
// Section types
const int SHT_PROGBITS = 1; // Program-defined contents.
const int SHT_SYMTAB = 2; // Symbol table.
const int SHT_STRTAB = 3; // String table.

const int SHT_RELA = 4; // Relocation entries; explicit addends.

const int SHT_HASH = 5; // Symbol hash table.

const int SHT_DYNAMIC = 6; // Information for dynamic linking.;

const int SHT_DYNSYM = 11; // Symbol table.;


class Ehdr {
	public:
		int16_t e_type; // Type of file (see ET_* below)
		int16_t e_machine; // Required architecture for this file (see EM_*)
		int32_t e_version; // Must be equal to 1

		int32_t e_flags; // Processor-specific flags
		int16_t e_ehsize; // Size of ELF header, in bytes
		int16_t e_phentsize; // Size of an entry in the program header table
		int16_t e_phnum; // Number of entries in the program header table
		int16_t e_shentsize; // Size of an entry in the section header table
		int16_t e_shnum; // Number of entries in the section header table
		int16_t e_shstrndx; // Sect hdr table index of sect name string table
		virtual int64_t getProgramOffset() = 0;
		virtual int64_t getSectionOffset() = 0;
};

class Elf_Phdr {
	public:
		int32_t p_type; // Type of segment
		int32_t p_offset; // File offset where segment is located, in bytes

		string flagsString() {
			string flags = "(";
			flags += ((getFlags() & PF_R) != 0 ? "R" : "_");
			flags += ((getFlags() & PF_W) != 0 ? "W" : "_");
			flags += ((getFlags() & PF_X) != 0 ? "X" : "_");
			flags += ")";
			return flags ;
		}

		virtual int64_t getFlags() = 0;

		virtual int64_t getVirtualAddr() = 0;

		virtual int64_t getFsize() = 0; //在文件中的大小
		virtual int64_t getMsize() = 0; //在内存中的大小

		string programType() {
			switch (p_type) {
				case PT_NULL:
					return "NULL";
				case PT_LOAD:
					return "Loadable Segment";
				case PT_DYNAMIC:
					return "Dynamic Segment";
				case PT_INTERP:
					return "Interpreter Path";
				case PT_NOTE:
					return "Note";
				case PT_SHLIB:
					return "PT_SHLIB";
				case PT_PHDR:
					return "Program Header";
				default:
					return "Unknown Section";
			}
		}
};

// Section header
class Elf_Shdr {
	public:
		int32_t sh_name; // Section name (index int32_to string table)
		int32_t sh_type; // Section type (SHT_*)
		int32_t sh_link; // Section type-specific header table index link
		int32_t sh_info; // Section type-specific extra information
		int32_t index;

		virtual int64_t getOffset() = 0;
		virtual int64_t getVirtualAddr() = 0;
		virtual int64_t getAddralign() = 0;
		virtual int32_t getSize() = 0;
		virtual int64_t getFlags() = 0;
};

class Elf_Sym {
	public:
		int32_t st_name; // Symbol name (index int32_to string table)
		char st_info; // Symbol's type and binding attributes
		char st_other; // Must be zero; reserved
		int16_t st_shndx; // Which section (header table index) it's defined in

		bool is_loaded; //是否已加载
		// These accessors and mutators correspond to the ELF32_ST_BIND,
		// ELF32_ST_TYPE, and ELF32_ST_INFO macros defined in the ELF
		// specification:
		Elf_Sym() {
			is_loaded = 0;
		}
		char getBinding() {
			return (char) (st_info >> 4);
		}
		virtual int64_t getSize() = 0;

		virtual int64_t getValue() = 0;

		char getType() {
			return (char) (st_info & 0x0f);
		}

		void setBinding(char b) {
			setBindingAndType(b, getType());
		}

		void setBindingAndType(char b, char t) {
			st_info = (char) ((b << 4) + (t & 0x0f));
		}

		void setType(char t) {
			setBindingAndType(getBinding(), t);
		}
};

class Elf_Rela {
	public:
		virtual int64_t getrOffset() = 0;
		virtual int64_t getrInfo() = 0;
};

class Elf32_Ehdr:public Ehdr {
	public:
		int32_t e_entry; // Address to jump to in order to start program
		int32_t e_phoff; // Program header table's file offset, in bytes
		int32_t e_shoff; // Section header table's file offset, in bytes


		virtual int64_t getProgramOffset() {
			return e_phoff;
		}

		virtual int64_t getSectionOffset() {
			return e_shoff;
		}
};

// Program header for ELF32.
class Elf32_Phdr:public Elf_Phdr {
	public:
		int32_t p_vaddr; // Virtual address of beginning of segment
		int32_t p_paddr; // Physical address of beginning of segment (OS-specific)
		int32_t p_filesz; // Num. of bytes in file image of segment (may be zero)
		int32_t p_memsz; // Num. of bytes in mem image of segment (may be zero)
		int32_t p_flags; // Segment flags
		int32_t p_align; // Segment alignment constraint32_t


		virtual int64_t getFlags() {
			return p_flags;
		}
		virtual int64_t getVirtualAddr() {
			return p_vaddr;
		}

		virtual int64_t getFsize() {
			return p_filesz;
		}

		virtual int64_t getMsize() {
			return p_memsz;
		}
};

class Elf32_Shdr:public Elf_Shdr {
	public:
		int32_t sh_flags; // Section flags (SHF_*)
		int32_t sh_addr; // Address where section is to be loaded
		int32_t sh_offset; // File offset of section data, in bytes
		int32_t sh_size; // Size of section, in bytes
		int32_t sh_addralign; // Section address alignment
		int32_t sh_entsize; // Size of records contained within the section

		virtual int64_t getAddralign() {
			return sh_addralign;
		}

		virtual int64_t getOffset() {
			return sh_offset;
		}

		virtual int64_t getVirtualAddr() {
			return sh_addr;
		}
		virtual int32_t getSize() {
			return sh_size;
		}

		virtual int64_t getFlags() {
			return sh_flags;
		}
};

// Symbol table entries for ELF32.
class Elf32_Sym:public Elf_Sym {
	public:
		int32_t st_value; // Value or address associated with the symbol
		int32_t st_size; // Size of the symbol


		virtual int64_t getSize() {
			return st_size;
		}
                virtual int64_t getValue() {
			return st_value;
		}
};

class Elf32_Rela:public Elf_Rela {
        public:
		int32_t r_offset;
		int32_t r_info;
                virtual int64_t getrOffset() {
			return r_offset;
		};
                virtual int64_t getrInfo() {
			return r_info;
		};
};

class Elf64_Ehdr:public Ehdr {
	public:
		int64_t e_entry;
		int64_t e_phoff;
		int64_t e_shoff;


		virtual int64_t getProgramOffset() {
			return e_phoff;
		}


		virtual int64_t getSectionOffset() {
			return e_shoff;
		}
};

// Program header for ELF64.
class Elf64_Phdr:public Elf_Phdr {
	public:
		int64_t p_vaddr; // Virtual address of beginning of segment
		int64_t p_paddr; // Physical address of beginning of segment (OS-specific)
		int64_t p_filesz; // Num. of bytes in file image of segment (may be zero)
		int64_t p_memsz; // Num. of bytes in mem image of segment (may be zero)
		int64_t p_flags; // Segment flags
		int64_t p_align; // Segment alignment constraint32_t


		virtual int64_t getFlags() {
			return p_flags;
		}
		virtual int64_t getVirtualAddr() {
			return p_vaddr;
		}
		virtual int64_t getFsize() {
			return p_filesz;
		}

		virtual int64_t getMsize() {
			return p_memsz;
		}
};

// Section header for ELF64 - same fields as ELF32, different types.
class Elf64_Shdr:public Elf_Shdr {
	public:
		int64_t sh_flags;
		int64_t sh_addr;
		int64_t sh_offset;
		int64_t sh_size;
		int64_t sh_addralign;
		int64_t sh_entsize;


		virtual int64_t getOffset() {
			return sh_offset;
		}
		virtual int64_t getVirtualAddr() {
			return sh_addr;
		}
		virtual int64_t getAddralign() {
			return sh_addralign;
		}
		virtual int32_t getSize() {
			return (int32_t) sh_size;
		}
		virtual int64_t getFlags() {
			return sh_flags;
		}
};

// Symbol table entries for ELF64.
class Elf64_Sym:public Elf_Sym {
	public:
		int64_t st_value; // Value or address associated with the symbol
		int64_t st_size; // Size of the symbol

		virtual int64_t getSize() {
			return st_size;
		}

                virtual int64_t getValue() {
                        return st_value;
                }

};

class Elf64_Rela:public Elf_Rela {
        public:
                int64_t r_offset;
                int64_t r_info;
		int64_t r_addend;
                virtual int64_t getrOffset() {
                        return r_offset;
                };
                virtual int64_t getrInfo() {
                        return r_info;
                };
};

class ELF {
	private:
		uint8_t* e_ident; // ELF Identification bytes
		Ehdr *mHeader;
		vector<Elf_Shdr*> mSectionHeaders;
		vector<Elf_Phdr*> mProgHeaders;
                vector<Elf_Rela*> mRelas;
                vector<Elf_Sym*> mDynamicSyms;
		char *mStringTable;
		int mStringTableLen;
		char *mDynStringTable;
                int mDynStringTableLen;
		void init();
		void copy(const ELF & elf);
	public:
                //LEDataInput *r;
		ELF(char *,size_t);
		ELF(const char *);
		ELF(const ELF & elf);
		ELF & operator=(const ELF & elf);
		~ELF();
		void parseELF(char *,size_t);
		bool checkMagic();
		bool isLittleEndian();
		uint8_t getDataEncoding();
		bool is64bit();
		uint8_t getFileClass();
		string getString(int index);
		Elf_Shdr *getSectionByName(string name);
		size_t getSectionAddr(string name);
		vector<Elf_Shdr*> SectionHeaders();
		vector<Elf_Phdr*> ProgHeaders();
                vector<Elf_Rela*> Relas();
                vector<Elf_Sym*> DynamicSyms();
		const char* getDynString(int index);
		size_t getSymAddr(string name);
};
#endif
