#include <memory_resource>
#include <memory>
#include <vector>
#include <QtCore/QtCore>
#include <iostream>
#include <QtGui/QtGui>
#include <optional>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <string_view>

using std::pmr::vector;
using std::pmr::list;
using namespace std::string_view_literals;

inline QString operator""_qu8(const char * a, std::size_t b) {
	return QString::fromUtf8(a, static_cast<int>(b));
}

inline std::ofstream & log() {
	static std::ofstream varStream{ "log.txt",std::ios::out };
	return varStream;
}

inline QString toVerb(const QString & arg) {
	//arg.replace(" "_qu8, "_"_qu8);
	if constexpr(false) {
		static const std::pmr::map<char, std::string_view > KeyMap = []() {
			std::pmr::map<char, std::string_view > varAns;
			varAns.emplace('#', u8R"(\# )"sv);
			varAns.emplace('$', u8R"(\$ )"sv);
			varAns.emplace('%', u8R"(\% )"sv);
			varAns.emplace('^', u8R"(\^{} )"sv);
			varAns.emplace('&', u8R"(\& )"sv);
			varAns.emplace('_', u8R"(\_ )"sv);
			varAns.emplace('{', u8R"(\{ )"sv);
			varAns.emplace('}', u8R"(\} )"sv);
			varAns.emplace('\\', u8R"(\textbackslash )"sv);
			varAns.emplace('~', u8R"(\~{} )"sv);
			return std::move(varAns);
		}();

		const auto var8 = arg.toUtf8();
		std::pmr::vector<char> varTmp;

		const auto varE = KeyMap.end();
		for (const auto & varI : var8) {
			auto varJ = KeyMap.find(varI);
			if (varJ == varE) {
				varTmp.push_back(varI);
			}
			else {
				varTmp.insert(varTmp.end(), varJ->second.begin(), varJ->second.end());
			}
		}

		return QString::fromUtf8(varTmp.data(), static_cast<int>(varTmp.size()));
	}
	else {
		static const std::pmr::map< char16_t, std::u16string_view > KeyMap = []() {
			std::pmr::map<char16_t, std::u16string_view > varAns;
			varAns.emplace(u'#', uR"(\# )"sv);
			varAns.emplace(u'$', uR"(\$ )"sv);
			varAns.emplace(u'%', uR"(\% )"sv);
			varAns.emplace(u'^', uR"(\^{} )"sv);
			varAns.emplace(u'&', uR"(\& )"sv);
			varAns.emplace(u'_', uR"(\_ )"sv);
			varAns.emplace(u'{', uR"(\{ )"sv);
			varAns.emplace(u'}', uR"(\} )"sv);
			varAns.emplace(u'\\', uR"(\textbackslash )"sv);
			varAns.emplace(u'~', uR"(\~{} )"sv);
			return std::move(varAns);
		}();

		const std::u16string_view var8{ reinterpret_cast<const char16_t  *>(arg.begin()),
			static_cast<std::size_t>(arg.size()) };

		std::pmr::vector<char16_t> varTmp;
		const auto varE = KeyMap.end();

		for (const auto & varI : var8) {
			auto varJ = KeyMap.find(varI);
			if (varJ == varE) {
				varTmp.push_back(varI);
			}
			else {
				varTmp.insert(varTmp.end(), varJ->second.begin(), varJ->second.end());
			}
		}

		return QString::fromUtf16(varTmp.data(), static_cast<int>(varTmp.size()));

	}
}

class Pack {
public:
	class FileInfo {
	public:

		int $Index = -1;
		QString $IndexName;
		QFileInfo $Info;
		bool $PDF = false;
		bool $ImageWith = false;
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
		QStringList() << "*.jpg" << "*.png" << "*.pdf",
		QDir::NoFilter,
		QDirIterator::Subdirectories);
	vector<QString> varDirNameAboutToDelete;
	while (varIt.hasNext()) {
		varIt.next();
		varDirNameAboutToDelete.push_back(varIt.fileInfo().absoluteFilePath());
	}
	for (const auto & varI : varDirNameAboutToDelete) {
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
		if (0 != varI.$Info.suffix().compare(QLatin1String("pdf", 3), Qt::CaseInsensitive)) {
			const QString varImageFileName = varI.$Info.absoluteFilePath();
			QImage varImage{ varImageFileName };
			if ((varImage.isNull()) || (varImage.height() < 1)) {
				const auto varFileName8 = varImageFileName.toLocal8Bit();
				log()
					<< "Image File Read Error! "
					<< varFileName8.data()
					<< std::endl;
				continue;
			}

			{
				constexpr const static auto K = 1.414;
				const auto k = double(varImage.width()) / double(varImage.height());
				varI.$ImageWith = (k > K);
			}

			varI.$Index = ++varIndex;
			varI.$IndexName = QString("%1").arg(varI.$Index, 10, 10, QChar('0'))
				+ QLatin1String(".jpg", 4);
			if (varImage.save(varDir->filePath(varI.$IndexName), "jpg") == false) {
				varI.$Index = -1;
			}

			varI.$PDF = false;
		}
		else {
			const QString varPdfFileName = varI.$Info.absoluteFilePath();
			varI.$Index = ++varIndex;
			varI.$IndexName = QString("%1").arg(varI.$Index, 10, 10, QChar('0'))
				+ QLatin1String(".pdf", 4);
			if (false == QFile::copy(varPdfFileName, varDir->filePath(varI.$IndexName))) {
				varI.$Index = -1;
			}
			varI.$PDF = true;
		}

	}

	QFile varFile{ varDir->absoluteFilePath("index.tex") };
	if (varFile.open(QIODevice::WriteOnly) == false) {
		log() << "create file fail!" << std::endl;
		return;
	}
	QTextStream varStream{ &varFile };
	varStream.setCodec(QTextCodec::codecForName("UTF-8"));
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
 


)!!"_qu8;


	bool isFirst = true;
	for (auto & varI : $Files) {
		if (varI.$Index < 0) {
			continue;
		}

		if (isFirst) {
			varStream << u8R"(
%280X190
 \noindent
 \parbox[c][\textheight][c]{\textwidth}{
 
\begin{center} 
)"_qu8;
			isFirst = false;
		}
		else {
			isFirst = true;
		}

		const auto varKeyName = varI.$Info.baseName();

		const static auto varBasicPdf = u8R"!!( %1 \\
\noindent
\resizebox{!}{14cm}{
%\rotatebox[origin=c]{ 0 }{
%	\includegraphics[width=19cm ]{%2}
%}
\includegraphics{%2}
} \\
)!!"_qu8;
		//natwidth=610,natheight=642
		const static auto varBasicImage = u8R"!!( %1 \\
\noindent
\resizebox{!}{14cm}{
%\rotatebox[origin=c]{ 0 }{
%	\includegraphics[width=19cm  ]{%2}
%}
\includegraphics{%2}
} \\
)!!"_qu8;
		const static auto varBasicImageW = u8R"!!( %1 \\
\noindent
\resizebox{\textwidth}{!}{
%\rotatebox[origin=c]{ 0 }{
%	\includegraphics[width=19cm  ]{%2}
%}
\includegraphics{%2}
} \\
)!!"_qu8;

		QString varData;
		if (varI.$PDF) {
			varData = varBasicPdf
				.arg(toVerb(varKeyName))
				.arg(varI.$IndexName);
		}
		else {
			if (varI.$ImageWith) {
				varData = varBasicImageW
					.arg(toVerb(varKeyName))
					.arg(varI.$IndexName);
			}
			else {
				varData = varBasicImage
					.arg(toVerb(varKeyName))
					.arg(varI.$IndexName);
			}
		}

		{
			varStream << std::as_const(varData);
		}

		if (isFirst) {
			varStream << u8R"(
\end{center}

}

\newpage 
)"_qu8;
		}


	}//for

	varStream << u8R"!!(

\end{document}

)!!"_qu8;
}

namespace {
	namespace DirIterator {
		class Next {
		public:
			list< QFileInfo > $Next;
			vector< QFileInfo > $Ans;
			Next(const QString & argDirName) {
				QDir varDir{ argDirName };
				const auto varList = varDir.entryInfoList(QDir::Dirs |
					QDir::Files |
					QDir::NoSymLinks |
					QDir::NoDotAndDotDot);
				for (const auto & varI : varList) {
					if (varI.isDir()) {
						const auto varDirNameX = varI.fileName();
						if (varDirNameX.compare(QLatin1Literal("tmp", 3),
							Qt::CaseInsensitive) == 0) {
							continue;
						}
						$Next.push_back(varI);
					}
					else {
						_add_to_ans(varI);
					}
				}
				_deep_search();
			}
		private:
			void _add_to_ans(const QFileInfo & arg) {
				const auto varSuffix = arg.suffix().toLower();
				const static QRegularExpression varExp{ u8R"(jpeg|png|jpg|pdf)"_qu8 };
				if (varExp.match(varSuffix).hasMatch()) {
					$Ans.push_back(arg);
				}
			}
			void _deep_search() {
				while (false == $Next.empty()) {
					const QFileInfo varInfo = std::move($Next.front());
					$Next.pop_front();
					const auto varDirName = varInfo.absoluteFilePath();
					const auto varList = QDir(varDirName)
						.entryInfoList(QDir::Dirs |
							QDir::Files |
							QDir::NoSymLinks |
							QDir::NoDotAndDotDot);
					for (const auto & varI : varList) {
						if (varI.isDir()) {
							$Next.push_back(varI);
						}
						else {
							_add_to_ans(varI);
						}
					}
				}
			}
		};
	}
}

inline void Pack::makeFiles() {

	$Files.clear();

	DirIterator::Next varNext{ $Dirname };
	{
		for (auto & varI : varNext.$Ans) {
			const auto varL8 = varI.absoluteFilePath().toLocal8Bit();
			log() << "find a name : " << varL8.data() << std::endl;
			$Files.emplace_back(std::move(varI));
		}
	}

}


