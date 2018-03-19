#include <memory_resource>
#include <memory>
#include <vector>
#include <QtCore/QtCore>
#include <iostream>
#include <QtGui/QtGui>
#include <optional>
#include <fstream>
using std::pmr::vector;

inline QString operator""_qu8(const char * a,std::size_t b) {
	return QString::fromUtf8(a, static_cast<int>(b));
}

inline std::ofstream & log() {
	static std::ofstream varStream{"log.txt",std::ios::out};
	return varStream;
}

class Pack {
public:
	class FileInfo {
	public:
		int $Index = -1;
		QString $IndexName;
		QFileInfo $Info;
		FileInfo() = default;
		FileInfo(QFileInfo && arg) :$Info(std::move(arg)) {}
	};

	QString $Dirname;
	vector< FileInfo > $Files;
	inline void makeFiles();
	inline void creakTex();
};

int main(int argc, char *argv[]) {

	QCoreApplication varApp{ argc,argv };
	/*this is a application , do not need to delete it;*/
	Pack * varPack = new Pack;

	if (argc < 2) {
		varPack->$Dirname = varApp.applicationDirPath();
	}
	else {
		varPack->$Dirname = QString::fromLocal8Bit(argv[1]);
	}

	varPack->makeFiles();
	varPack->creakTex();
	log().flush();
}

inline void rmTmpFile(const QString & argDirName) {
	QDirIterator varIt(argDirName,
		QStringList() << "*.tmp" <<"*.pdf" ,
		QDir::NoFilter,
		QDirIterator::Subdirectories);
	vector<QString> varDirNameAboutToDelete;
	while (varIt.hasNext()) {
		varIt.next();
		varDirNameAboutToDelete.push_back( varIt.fileInfo().absoluteFilePath());
	}
	for (const auto & varI:varDirNameAboutToDelete) {
		QFile::remove(varI);
	}
}

inline void Pack::creakTex() {

	std::optional<QDir> varDir;
	do {
		QDir varDirTmp;
		const auto varTmpDN = varDirTmp.absoluteFilePath(QLatin1String("tmp", 3));
		if (QFileInfo::exists(varTmpDN)) {
			varDir = QDir{ varTmpDN };
			break;
		}
		if (varDirTmp.mkpath(varTmpDN)) {
			varDir = QDir{ varTmpDN };
			break;
		}
		log() << "can not create dir tmp!" << std::endl;
		return;
	} while (false);

	rmTmpFile(varDir->absolutePath());

	int varIndex = 0;
	for (auto & varI : $Files) {
		if (0 == varI.$Info.suffix().compare(QLatin1String("pdf", 3), Qt::CaseInsensitive)) {
			const QString varImageFileName = varI.$Info.absoluteFilePath();
			QImage varImage{ varImageFileName };
			if (varImage.isNull()) {
				const auto varFileName8 = varImageFileName.toLocal8Bit();
				log()
					<< "Image File Read Error! "
					<< varFileName8.data()
					<< std::endl;
				continue;
			}
			varI.$Index = ++varIndex;
			varI.$IndexName = QString("%1").arg(varI.$Index, 10, 10, QChar('0'))
				+ QLatin1String(".tmp", 4);
			if (varImage.save(varDir->filePath(varI.$IndexName), "png") == false) {
				varI.$Index = -1;
			}
		}
		else {
			const QString varPdfFileName = varI.$Info.absoluteFilePath();
			varI.$Index = ++varIndex;
			varI.$IndexName = QString("%1").arg(varI.$Index, 10, 10, QChar('0'))
				+ QLatin1String(".tmp", 4);
			if (false == QFile::copy(varPdfFileName, varDir->filePath(varI.$IndexName))) {
				varI.$Index = -1;
			}
		}			

	}

	QFile varFile{ varDir->absoluteFilePath("index.tex") };
	if (varFile.open(QIODevice::WriteOnly)==false) {
		log() << "create file fail!" << std::endl;
		return;
	}
	QTextStream varStream{&varFile};
	varStream.setCodec( QTextCodec::codecForName("utf8") );
	varStream.setGenerateByteOrderMark(false);

	varStream << u8R"!!(
\documentclass[hyperref,UTF8]{ctexart}

%设置页面
\usepackage[
	a4paper,
	left=0.1cm,
	right=0.1cm,
	top=0.1cm,
	bottom=0.1cm,
	%landscape,
]
{geometry}

%设置输出pdf格式
\usepackage[
	colorlinks=true ,
	%bookmarks=true,
	%bookmarksopen=false,
	%pdfpagemode=FullScreen,
	%pdfstartview=Fit,
	pdftitle={蛊真人} ,%标题
	pdfauthor={蛊真人} ,%作者
	pdfsubject={蛊真人} ,%主题
	pdfkeywords={蛊真人} , %关键字
	linkcolor=black 
]{hyperref}  

\usepackage{graphicx}
\usepackage{float}
\usepackage{amsmath}
\usepackage{cite}
\usepackage{caption}
%\usepackage{titlesec}
\usepackage{setspace}

%图片相关
\usepackage{graphicx}

%页码 页眉 页脚
\pagestyle{empty}

\begin{document}
 
%280X190
 \noindent
 \parbox[c][\textheight][c]{\textwidth}{
 
\begin{center} 

)!!"_qu8;

	for (auto & varI : $Files) {
		if (varI.$Index < 0) {
			continue; 
		}

		const auto varKeyName = varI.$Info.baseName();

		const auto varBasic = u8R"!!( %1 \\
\noindent
\resizebox{!}{14cm}{
\rotatebox[origin=c]{ 0 }{
	\includegraphics[width=19cm]{%2}
}
} \\
)!!"_qu8;

		const auto varData = varBasic.arg(varKeyName).arg( varI.$IndexName );
		{
			const auto varDT = varData.toUtf8();
			varStream << varDT.data();
		}
	}

	varStream << u8R"!!(
\end{center}

}

\newpage 

 
\end{document}

)!!"_qu8;
}

inline void Pack::makeFiles() {
	QDirIterator varIt($Dirname,
		QStringList() << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.pdf",
		QDir::NoFilter,
		QDirIterator::Subdirectories);
	$Files.clear();
	while (varIt.hasNext()) {
		const auto varL8 = varIt.next().toLocal8Bit();
		$Files.emplace_back(varIt.fileInfo());
		log() << "find a name : " << varL8.data() << std::endl;
	}
}

