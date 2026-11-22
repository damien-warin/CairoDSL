#include <CairoCompilerLib.h>
#include <QFileInfo>


#include <OpProgram.h>

extern "C"
{


#include <stdlib.h>
#include <stdio.h>
#include <CanvaCtx.h>
#include <OpCanva.h>
#include <parser.tab.h>
#include <lexer.h>
#include <unistd.h>
#include <log.h>


extern int yycolumn;
extern int yylineno;
int cairo_parse(String *buffer, String *prefix, String *out, QList<struct MessageItem> &msgs, int *is_xml, int *width, int *height)
{
	log_set_level(LOG_INFO);
	CanvaCtx Canva;
	size_t readed;
	OpCanvaContext Ctx;
	OpProgram Prog;
	OpCanvaContext_init(&Ctx);
	OpProgram_init(&Prog);
	OpParser Parser;
	OpParser_init(&Parser);
	OpParser_set_filename_prefix(&Parser, prefix);
	OpProgram_set_context(&Prog, (OpContext*)&Ctx);
	OpParser_set_program(&Parser, &Prog);
	OpParser_push_current_context(&Parser, (OpContext*)&Ctx);
	yycolumn = 1;
	yylineno = 1;
	/*extern int yydebug, yy_flex_debug;
	yydebug = 1;
	yy_flex_debug = 1;*/
	log_trace("Code String : %s\n", String_get_char_string(buffer));

	if(OpParser_parse(&Parser, buffer) == 0)
	{
		int w, h;

		OpProgram_prerun(&Prog);


		w = OpCanvaContext_get_width(&Ctx);
		h = OpCanvaContext_get_height(&Ctx);
		*width = (w != 0 ? w : 640);
		*height = (h != 0 ? h : 480);

		CanvaCtxOutputMode output_mode = OpCanvaContext_get_output_mode(&Ctx);

		if(output_mode == PNG)
		{
			CanvaCtx_init_for_png(&Canva, *width, *height);

			CanvaCtx_set_line_width (&Canva, 1.0);
			CanvaCtx_set_color(&Canva, 0, 0, 0, 255);

			OpCanvaContext_set_Canva(&Ctx, &Canva);
			OpProgram_set_Canva(&Prog, &Canva);

			OpProgram_run(&Prog);

			CanvaCtx_write_to_png(&Canva);

			CanvaCtx_fill_buffer_with_output(&Canva, out);
			*is_xml = 0;

			CanvaCtx_terminate(&Canva);
		}

		if(output_mode == SVG)
		{
			CanvaCtx_init_for_svg(&Canva, *width, *height);

			CanvaCtx_set_line_width (&Canva, 1.0);
			CanvaCtx_set_color(&Canva, 0, 0, 0, 255);

			OpCanvaContext_set_Canva(&Ctx, &Canva);
			OpProgram_set_Canva(&Prog, &Canva);

			OpProgram_run(&Prog);

			CanvaCtx_finish(&Canva);
			CanvaCtx_fill_buffer_with_output(&Canva, out);

			*is_xml = 1;
			CanvaCtx_terminate(&Canva);
		}
	}

	LinkedListIterator *ite = OpProgram_get_messages(&Prog);
    while(!LinkedListIterator_is_eol(ite))
    {
    	LinkedListIterator_go_to_next(ite);
    	OpMessage *m = (OpMessage*)LinkedListIterator_get_current_value(ite);
    	if(m != NULL)
    	{
    		MessageItem i;
    		i.filename = String_get_char_string(&m->filename);
    		i.firstLine = m->pos.first_line;
    		i.firstColumn = m->pos.first_column;
    		i.lastLine = m->pos.last_line;
    		i.lastColumn = m->pos.last_column;
    		i.state = m->state;
    		i.text = String_get_char_string(&m->msg);
    		msgs.append(i);
    	}
    }
    LinkedListIterator_free(ite);

	OpCanvaContext_terminate(&Ctx);
	OpProgram_terminate(&Prog);
	OpParser_terminate(&Parser);
	return 0;
}
}

// Méthode simulée qui prend le code DSL et retourne le XML
int processlib(const QString& code, const QString &filepath, QList<struct MessageItem> &msgs, QByteArray &output, bool &is_xml, int *width, int *height) {

	QByteArray ba = code.toUtf8();

	const char *c_str = ba.constData(); // ou ba.data()
	size_t length = static_cast<size_t>(ba.size());
	String *scode = String_new();
	String_append_data(scode, length, (const void*)c_str);
	String path;
	String_init(&path);
	QString dirNameAbs = QFileInfo(filepath).absolutePath();
	QByteArray bap = dirNameAbs.toUtf8();

	const char *p_str = bap.constData(); // ou ba.data()
	size_t p_length = static_cast<size_t>(bap.size());
	String_append_data(&path, p_length, (const void*)p_str);

	String out;
	// Remplace ce stub par l'appel effectif à ton moteur/bibliothèque.
	// Exemple de réponse XML retournée par la lib :
	String_init(&out);
	qDebug() << "Code : " << code;
	qDebug() << "Code Array : " << ba;
	int is_xml_int;
	int ret = cairo_parse(scode, &path, &out, msgs, &is_xml_int, width, height);

	if(ret == 0)
	{
		is_xml_int == 1 ? is_xml = true : is_xml = false;

		output = QByteArray(String_get_data(&out), static_cast<qsizetype>(String_get_length(&out)));
	}
	String_finalize(&out);
	String_finalize(&path);

	return ret;

}
