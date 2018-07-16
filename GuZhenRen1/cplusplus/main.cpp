#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <memory>
#include <memory_resource>
#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <string_view>
#include <optional>

/*仅支持ASCII码*/
inline const  QDir & $g$ProjectCurrentPath() {
	static const auto $m$Ans = QDir{ PRO_CURRENT_PATH };
	return $m$Ans;
}

/*将替换latex特殊字符*/
inline std::string replace_all(const std::string_view arg) {
	if (arg.empty()) { return {}; }

	using namespace std::string_view_literals;
	class ReplaceItem {
	public:
		std::regex regex;
		std::string data;
		ReplaceItem() = default;
		ReplaceItem(std::regex && a, std::string &&b) :regex(std::move(a)), data(std::move(b)) {}
	};

	//regex : ^ $ \ . * + ? ( ) [ ] { } |
	const static std::vector< ReplaceItem > varReplaceDutys = []() {
		std::vector< ReplaceItem > ans;
		ans.emplace_back(std::regex(u8R"(~)"), std::string(u8R"(\~{})"sv));
		ans.emplace_back(std::regex(u8R"(#)"), std::string(u8R"(\#)"sv));
		ans.emplace_back(std::regex(u8R"(\$)"), std::string(u8R"(\$)"sv));
		ans.emplace_back(std::regex(u8R"(%)"), std::string(u8R"(\%)"sv));
		ans.emplace_back(std::regex(u8R"(\^)"), std::string(u8R"(\^{})"sv));
		ans.emplace_back(std::regex(u8R"(&)"), std::string(u8R"(\&)"sv));
		ans.emplace_back(std::regex(u8R"(\{)"), std::string(u8R"(\{)"sv));
		ans.emplace_back(std::regex(u8R"(\})"), std::string(u8R"(\})"sv));
		ans.emplace_back(std::regex(u8R"(_)"), std::string(u8R"(\_)"sv));
		ans.emplace_back(std::regex(u8R"(\\)"), std::string(u8R"(\\)"sv));
		return std::move(ans);
	}();

	std::string ans;
	class Replace {
	public:
		std::string_view data;
		bool is_replace = false;
		Replace(std::string_view a, bool b) :data(a), is_replace(b) {}
		Replace() = default;
	};
	std::vector<Replace> tmpReplacesInput;
	std::vector<Replace> tmpReplaces;

	tmpReplaces.emplace_back(arg, false);

	for (const auto & R : varReplaceDutys) {
		tmpReplacesInput = std::move(tmpReplaces);

		for (const auto & varD : tmpReplacesInput) {

			if (varD.is_replace) { /*copy the replaced data to ans*/
				tmpReplaces.push_back(varD);
				continue;
			}

			if (varD.data.empty()) { /*skip the empty data  */
				//tmpReplaces.push_back(varD);
				continue;
			}

			std::regex_iterator varIt{ varD.data.data() ,varD.data.data() + varD.data.size() , R.regex };
			decltype(varIt) varE;

			if (varIt == varE) { /*if data do not match regex , copy it to ans*/
				tmpReplaces.push_back(varD);
				continue;
			}

			/*add a do not used data,it will be poped*/
			tmpReplaces.emplace_back(std::string_view{}, false);
			for (; varIt != varE; ++varIt) { /*replace the data*/
				tmpReplaces.pop_back();
				const auto varPreFix = varIt->prefix();
				const std::string_view varBeforeP(varPreFix.first, varPreFix.length());
				const auto varSubFix = varIt->suffix();
				const std::string_view varAfterP(varSubFix.first, varSubFix.length());
				tmpReplaces.emplace_back(varBeforeP, false);
				tmpReplaces.emplace_back(R.data, true);
				tmpReplaces.emplace_back(varAfterP, false);
			}

		}/*replace*/
	}/*regex*/

	for (const auto & varI : tmpReplaces) {
		if (varI.data.empty())continue;
		ans.append(varI.data);
	}

	return std::move(ans);
}

/*获得章名，去掉 第……卷 */
inline std::string get_chapter_name(const std::string & arg) {
	if (arg.empty()) { return {}; }
	const static std::regex varR{ u8R"(^第(一|二|三|四|五|六|七|八|九|十|零|百|千|万)+?卷(( |:|：)+)(.*))" };
	std::smatch varAns;
	if (std::regex_match(arg, varAns, varR)) {
		if (varAns.length() < 5) { return{}; }
		return replace_all(varAns[4].str());
	}
	return{};
}

/*获得节名，去掉 第……节 */
inline std::string get_section_name(const std::string & arg) {
	if (arg.empty()) { return {}; }
	const static std::regex varR{ u8R"(^第(一|二|三|四|五|六|七|八|九|十|零|百|千|万)+?节(( |:|：)+)(.*))" };
	std::smatch varAns;
	if (std::regex_match(arg, varAns, varR)) {
		if (varAns.length() < 5) { return{}; }
		return replace_all(varAns[4].str());
	}
	return{};
}

class Chapter {
public:
	std::vector<std::string> par_data;
	std::string par_chapter_name;
};

class DetailChapter {
public:
	std::string par_chapter_name;
	class Section {
	public:
		std::string par_section_name;
		std::vector<std::string> par_data;
	};
	std::vector< Section> par_sections;
};

/*分节*/
inline std::vector< DetailChapter > split_sectoin(std::vector<Chapter> & arg) {
	std::vector< DetailChapter > varAns;
	varAns.reserve(arg.size());
	const static std::regex varR{ u8R"(^第(一|二|三|四|五|六|七|八|九|十|零|百|千|万)+?节( |:|：).*)" };
	for (auto & varI : arg) {
		DetailChapter & varC = varAns.emplace_back();
		varC.par_chapter_name = std::move(varI.par_chapter_name);
		DetailChapter::Section varCurrentSection;
		for (auto & varL : varI.par_data) {
			if (std::regex_match(varL, varR)) {
				if (varCurrentSection.par_section_name.empty()) {
					varCurrentSection.par_section_name = std::move(varL);
					varCurrentSection.par_data.clear();
				}
				else {
					/*删除少于10行的数据？？？*/
					if (varCurrentSection.par_data.size() > 10) {
						varC.par_sections.push_back(std::move(varCurrentSection));
					}
					varCurrentSection.par_section_name = std::move(varL);
				}
			}
			else {
				varCurrentSection.par_data.push_back(std::move(varL));
			}
		}/*for*/
		if (!varCurrentSection.par_section_name.empty()) {
			varC.par_sections.push_back(std::move(varCurrentSection));
		}
	}/*for*/
	return std::move(varAns);
}

/*分章*/
inline std::vector< Chapter > split_chapter(std::vector<std::string> & arg) {
	std::vector< Chapter > varAns;
	const static std::regex varR{ u8R"(^第(一|二|三|四|五|六|七|八|九|十|零|百|千|万)+?卷( |:|：).*)" };
	//goto_next:
	{
		Chapter varChapter;
		for (auto & varI : arg) {
			//qDebug() << QString::fromUtf8(varI.c_str());
			if (std::regex_match(varI, varR)) {
				//qDebug() << QString::fromUtf8(varI.c_str());
				if (varChapter.par_chapter_name.empty()) {
					varChapter.par_data.clear();
					varChapter.par_chapter_name = std::move(varI);
				}
				else {
					varAns.push_back(std::move(varChapter));
					varChapter.par_chapter_name = std::move(varI);
				}
			}
			else {
				varChapter.par_data.push_back(std::move(varI));
			}
		}/*for*/
		if (!varChapter.par_chapter_name.empty())
			varAns.push_back(std::move(varChapter));
	}
	return std::move(varAns);
}

/*去掉原文带的广告*/
inline void remove_ad(std::vector< DetailChapter > & arg) {
	const static std::regex varR{ u8R"(-*)" };

	/*移除文档末尾的 ------------ */
	for (auto & varC : arg) {
		for (auto & varS : varC.par_sections) {
			auto & varD = varS.par_data;
			for (;;) {
				if (varD.empty()) { break; }
				const auto & l = *(varD.rbegin());
				if (std::regex_match(l, varR)) {
					varD.pop_back();
					continue;
				}
				break;
			}
		}
	}

}

/*根据guzhenren.txt生成tex文件*/
inline void update() {
	using namespace std::string_view_literals;
	std::vector< DetailChapter > detail_chapters;
	{
		std::vector< Chapter > chapters;
		{
			std::vector<std::string> lines;
			{
				QFile varFile{ $g$ProjectCurrentPath().absoluteFilePath(QStringLiteral("../guzhenren.txt")) };
				varFile.open(QIODevice::ReadOnly);
				QTextStream varStream{ &varFile };
				while (varStream.atEnd() == false) {
					const auto varLine = varStream.readLine().trimmed();
					if (varLine.isEmpty()) { continue; }
					lines.push_back(varLine.toUtf8().toStdString());
				}
			}
			chapters = split_chapter(lines);
		}
		detail_chapters = split_sectoin(chapters);
	}

	remove_ad(detail_chapters);

	std::uint32_t n_chapter = 0;
	std::uint32_t n_section = 0;
	std::optional< std::filesystem::path > log_file_path;
	const QString varDirRoot = $g$ProjectCurrentPath().absoluteFilePath("../");
	{
		QDir varDir{ varDirRoot };
		log_file_path = std::filesystem::u8path(
			varDir.absoluteFilePath(QStringLiteral("log.txt"))
			.toUtf8()
			.toStdString());
	}

	std::ofstream log_file{ *log_file_path,std::ios::binary };

	for (const auto & varC : detail_chapters) {
		n_section = 0;
		++n_chapter;
		QString dir_path;
		const QString varChapterName_ = QString("chapter_%1").arg(n_chapter, 2, 10, QChar('0'));
		{
			QDir varDir{ varDirRoot };
			dir_path = varDir.absoluteFilePath(varChapterName_);
			varDir.mkpath(dir_path);
		}
		QDir varDir{ dir_path };

		//create : index.tex
		std::ofstream ofs_chapter{
			std::filesystem::u8path(varDir.absoluteFilePath(QStringLiteral("index.tex")).toUtf8().toStdString()),
			std::ios::binary
		};

		ofs_chapter << u8R"(\newchapter{)"sv
			<< get_chapter_name(varC.par_chapter_name)
			<< u8R"(})"sv
			<< u8R"(    %)"sv
			<< varC.par_chapter_name
			<< std::endl
			<< std::endl;

		const auto varChapterName = varChapterName_.toUtf8().toStdString();
		for (const auto & varS : varC.par_sections) {
			const QString sectionName__ = QString("%1").arg(++n_section, 3, 10, QChar('0')) + QStringLiteral(".tex");
			const QString sectionPath = varDir.absoluteFilePath(sectionName__);
			const auto setctionName = sectionName__.toUtf8().toStdString();
			const std::filesystem::path sectionPathFile = std::filesystem::u8path(sectionPath.toUtf8().toStdString());
			std::ofstream ofs_sectoin{
				sectionPathFile,std::ios::binary
			};

			//\input{chapter_01/section_001.tex}
			ofs_chapter << u8R"(\input{)"sv;
			ofs_chapter << varChapterName;
			ofs_chapter << u8R"(/)"sv;
			ofs_chapter << setctionName;
			ofs_chapter << u8R"(})"sv;
			ofs_chapter << std::endl;
			/***
			\newsection{ chapter_name } %old chapter_name
			\begin{this_body}
			dataline \par
			\end{this_body}
			***/

			ofs_sectoin << u8R"(\newsection{)"sv
				<< get_section_name(varS.par_section_name)
				<< u8R"(})"sv
				<< u8R"(    %)"sv
				<< varS.par_section_name
				<< std::endl
				<< std::endl;
			ofs_sectoin << u8R"(\begin{this_body})"sv << std::endl << std::endl;
			std::uint32_t varLineCout = 0;
			/*写每一行*/
			for (const auto & varP : varS.par_data) {
				ofs_sectoin
					<< u8R"(%)"sv
					<< ++varLineCout
					<< std::endl
					<< replace_all(varP)
					<< std::endl
					<< std::endl;
				/************************************************/
				//如果改行有可能的广告字符，则将改行输出到log文件
				const static std::regex varRegex{ u8R"([*a-zA-Z.])" };
				const static std::regex varIgnoreRegex{u8R"(^(ps|（|\()[\s\S]+$)"};
				if (std::regex_search(varP, varRegex)) {
					if (!std::regex_match(varP,varIgnoreRegex)) {
						log_file
							<< varC.par_chapter_name
							<< u8R"(;)"sv
							<< std::endl
							<< varS.par_section_name
							<< u8R"(;)"sv
							<< std::endl
							<< varLineCout
							<< u8R"(;)"sv
							<< std::endl
							<< varP
							<< std::endl
							<< std::endl;
					}
				}
				/************************************************/
			}
			ofs_sectoin << u8R"(\end{this_body})"sv << std::endl << std::endl;
		}

	}

}

/*write on : 2018/7/16*/
int main(int argc, char *argv[]) {

	QGuiApplication app(argc, argv);

	update();

	return 0;
}

