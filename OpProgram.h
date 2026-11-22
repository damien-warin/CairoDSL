/*
 * ops.h
 *
 *  Created on: 10 juil. 2026
 *      Author: damien
 */

#ifndef OPPROGRAM_H_
#define OPPROGRAM_H_

#include <libxml/xmlreader.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <Op.h>
#include <OpModule.h>
#include <LinkedList.h>
#include <String.h>
#include <uthash.h>

typedef struct op_import OpImport;

typedef struct op_program
{
	OpContext *ctx;//Main context
	Op *root;//Main program
	OpModule **modules;
	size_t number_of_modules;
	OpModule **functions;
	size_t number_of_functions;
	OpImport **imports;
	size_t number_of_imports;
	LinkedList messages;
}OpProgram;

typedef struct yy_buffer_state *YY_BUFFER_STATE;

typedef struct op_parser_file
{
	YY_BUFFER_STATE state;
	String *file_content;
	int column;
	int line;
}OpParserFile;

typedef struct op_parser
{
	OpContext **contexts;
	size_t nb_contexts;
	size_t cur_context_nb;
	OpModule *current_m;
	OpModule *current_f;
	ssize_t current_pattern;
	int inside_module;
	int inside_function;
	OpProgram *program;
	OpParserFile **files;
	LinkedList filenames;
	size_t files_size;
	String filename_prefix;
	int depth;
	int inside_loop;
	size_t auto_gen_vars;
	Op *temp;
}OpParser;

typedef struct op_parser_hash_elt
{
	char name[30];
	char value[100];
	UT_hash_handle hh;
}OpParserHashElt;

typedef struct op_import
{
	Op *root;
	Op *parser;
	size_t x;
	size_t y;
	size_t w;
	size_t h;
}OpImport;

typedef struct op_parse_import
{
	Op super;
	String prefix;
	Op *name;
	OpProgram *program;
}OpParseImport;

typedef struct op_draw_import
{
	Op super;
	Op *number;
	OpProgram *program;

}OpDrawImport;

typedef struct op_get_import_extents
{
	Op super;
	Op *number;
	OpProgram *program;

}OpGetImportExtents;


#include <OpProgram.proto.h>

#ifdef __cplusplus
}
#endif

#endif /* OPPROGRAM_H_ */
