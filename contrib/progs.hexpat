#pragma author erysdren
#pragma description QuakeC Progs
#pragma endian little

enum ProgsVersion : u32 {
	QTEST = 3,
	QUAKE = 6,
	FTE = 7
};

enum ProgsSecondaryVersion : u32 {
	FTE16 = 0x021B1461,
	FTE32 = 0x65167402
};

enum ProgsCrc : u32 {
	QTEST = 58535,
	QUAKE091 = 6956,
	QUAKE101 = 5927,
	QUAKEWORLD = 54730
};

struct ProgsFunction {
	if (header.version == ProgsVersion::QTEST) {
		u32 first_statement;
		padding[12];
		u32 ofs_name;
		u32 ofs_filename;
		u32 num_parms;
		u32 first_parm;
		u32 len_parms;
	} else {
		u32 first_statement;
		u32 first_parm;
		u32 num_locals;
		u32 profile;
		u32 ofs_name;
		u32 ofs_filename;
		u32 num_parms;
		u8 len_parms[8];
	}

	char name[] @ header.ofs_strings + ofs_name;
	char filename[] @ header.ofs_strings + ofs_filename;
};

struct ProgsStatement {
	if (header.version == ProgsVersion::FTE) {
		if (header.secondary_version == ProgsSecondaryVersion::FTE32) {
			u32 opcode;
			u32 a;
			u32 b;
			u32 c;
		}
	} else {
		if (header.version == ProgsVersion::QTEST) {
			u32 line;
		}
		u16 opcode;
		s16 a;
		s16 b;
		s16 c;
	}
};

struct ProgsDef {
	if (header.version == ProgsVersion::FTE) {
		if (header.secondary_version == ProgsSecondaryVersion::FTE32) {
			u32 type;
			u32 ofs;
			u32 ofs_name;
		}
	} else if (header.version == ProgsVersion::QTEST) {
		u32 type;
		u32 ofs_name;
		u32 ofs;
	} else {
		u16 type;
		u16 ofs;
		u32 ofs_name;
	}

	char name[] @ header.ofs_strings + ofs_name;
};

union ProgsGlobal {
	s32 i;
	u32 ui;
	float f;
};

struct ProgsHeader {
	ProgsVersion version;
	ProgsCrc crc;
	u32 ofs_statements;
	u32 num_statements;
	u32 ofs_global_defs;
	u32 num_global_defs;
	u32 ofs_field_defs;
	u32 num_field_defs;
	u32 ofs_functions;
	u32 num_functions;
	u32 ofs_strings;
	u32 len_strings;
	u32 ofs_globals;
	u32 num_globals;
	u32 num_entity_fields;

	// extended header
	if (version == ProgsVersion::FTE) {
		u32 ofs_files;
		u32 ofs_linenums;
		u32 ofs_compressed_functions;
		u32 num_compressed_functions;
		u32 ofs_types;
		u32 num_types;
		u32 num_compressed_blocks;
		ProgsSecondaryVersion secondary_version;
	}
};

ProgsHeader header @ 0x00;
char strings[header.len_strings] @ header.ofs_strings;
ProgsStatement statements[header.num_statements] @ header.ofs_statements;
ProgsFunction functions[header.num_functions] @ header.ofs_functions;
ProgsDef global_defs[header.num_global_defs] @ header.ofs_global_defs;
ProgsDef field_defs[header.num_field_defs] @ header.ofs_field_defs;
ProgsGlobal globals[header.num_globals] @ header.ofs_globals;
