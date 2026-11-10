#include <QApplication>
#include <QLoggingCategory>
#include "mainwindow.h"
#include <QTranslator>
#include <QLocale>

#include <OpProgram.h>

extern "C"
{
#include <CanvaCtx.h>
#include <OpCanva.h>
#include <parser.tab.h>
}

int main(int argc, char *argv[]) {
    std::setlocale(LC_ALL, "C");//Rétablie la locale "C" globale pour le parsing "classique" des nombre à virgule.

	QLoggingCategory::setFilterRules(
	        "*.debug=false\n"
	        "*.trace=false\n"
	    );

    QApplication app(argc, argv);

    QTranslator translator;

	// Vérifier si la langue système est le français
	if (QLocale::system().language() == QLocale::French) {
		// Charger le fichier de traduction français (.qm)
		// (Le fichier peut être dans les ressources Qt "qrc" ou sur le disque)
		bool loaded = translator.load(":/i18n/cairodsl.qm");
		if (loaded) {
			app.installTranslator(&translator);
			qDebug() << "Langue française chargée avec succès.";
		} else {
			qDebug() << "Échec du chargement de la traduction française, passage à l'anglais par défaut.";
		}
	} else {
		qDebug() << "Langue système non française, affichage en anglais par défaut.";
	}


    MainWindow w;
    w.setWindowTitle("Cairo DSL Editor");
    w.show();

    return app.exec();
}
