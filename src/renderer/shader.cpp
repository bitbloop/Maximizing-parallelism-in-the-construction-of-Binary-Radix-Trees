
#include "renderer.h"
#include <fstream>
#include <iostream>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

// ------------------------------------------------------------------------------------------------------------------------------------------

namespace
{
	// trim from start
	static inline std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// trim from end
	static inline std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	// trim from both ends
	static inline std::string &trim(std::string &s) {
		return ltrim(rtrim(s));
	}

	// trim the double quatation marks "
	static inline std::string& qtrim(std::string& s) {
		s.erase(std::remove(s.begin(), s.end(), '"'), s.end());
		return s;
	}



	/** */
	void printShaderLog(const int shaderId, const std::vector<std::string*> code_set)
	{
		int logLength;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 1) {
			std::string complete_code = "";
			for (const std::string* file : code_set)
				complete_code.append(*file);

			char *log = new char[logLength];
			glGetShaderInfoLog(shaderId, logLength, &logLength, log);
			std::cout << log << std::endl;
			delete[] log;
		}
	}


	/*
	**
	*/
	bool has_main_function(const std::vector<std::string*>& code_set)
	{
		for (auto str : code_set)
		{
			if (str->find("main()") != std::string::npos ||
				str->find("main ()") != std::string::npos)
				return true;
		}

		return false;
	}


	/*
	**
	*/
	void load_file(std::string& complete_code, const std::string& file_name, const std::unordered_set<std::string>* defines_in, const std::unordered_map<std::string, std::string>* define_subs_in)
	{
		std::ifstream myfile;
		myfile.open(file_name, std::ifstream::in);
		if (myfile.is_open())
		{
			std::unordered_set<std::string> defines = (defines_in == nullptr) ? std::unordered_set<std::string>() : *defines_in; // this will make a copy every call but it was fun to code this :D hope it works!! :D
			std::unordered_map<std::string, std::string> define_subs = (define_subs_in == nullptr) ? std::unordered_map<std::string, std::string>() : *define_subs_in; // this will make a copy every call but it was fun to code this :D hope it works!! :D


			const auto _last_path_bracket_pos_1 = file_name.find_last_of("\\/");
			const auto _last_path_bracket_pos_2 = file_name.find_last_of("\\/");

			const auto _last_path_bracket_pos = (_last_path_bracket_pos_1 != std::string::npos) ? _last_path_bracket_pos_1 :
				(_last_path_bracket_pos_2 != std::string::npos) ? _last_path_bracket_pos_2 : 0;

			const std::string base_path = file_name.substr(0, _last_path_bracket_pos+1);

			std::string line;
			bool done = false;
			bool inside_bad_define = false;
			unsigned int bad_define_depth = 0;

			while (!done)
			{
				if (!std::getline(myfile, line))
				{
					done = true;
					continue;
				}

				line = line.substr(0, line.find("//"));

				size_t ifdef_pos = line.find("#ifdef ");
				if (ifdef_pos != std::string::npos)
				{
					std::string define_name = line.substr(line.find(" ", ifdef_pos) + 1);
					rtrim(define_name);

					if (inside_bad_define)
					{
						++bad_define_depth;
					}
					else if (defines.find(define_name) == defines.end())
					{
						inside_bad_define = true;
						++bad_define_depth;
					}
					continue;
				}

				size_t ifndef_pos = line.find("#ifndef ");
				if (ifndef_pos != std::string::npos)
				{
					std::string define_name = line.substr(line.find(" ", ifndef_pos) + 1);
					rtrim(define_name);

					if (inside_bad_define)
					{
						++bad_define_depth;
					}
					else if (defines.find(define_name) != defines.end())
					{
						inside_bad_define = true;
						++bad_define_depth;
					}
					continue;
				}

				size_t endif_pos = line.find("#endif");
				if (endif_pos != std::string::npos)
				{
					if (inside_bad_define) --bad_define_depth;
					if (bad_define_depth == 0) inside_bad_define = false;
					continue;
				}

				if (inside_bad_define) continue;

				size_t include_pos = line.find("#include ");
				if (include_pos != std::string::npos)
				{
					std::string include_file_name = line.substr(line.find(" ", include_pos) + 1);
					rtrim(include_file_name);
					qtrim(include_file_name);
					load_file(complete_code, base_path+include_file_name, &defines, &define_subs);
					continue;
				}

				size_t define_pos = line.find("#define ");
				if (define_pos != std::string::npos)
				{
					std::string define_name = line.substr(line.find(" ", define_pos) + 1);
					rtrim(define_name);
					qtrim(define_name);
					defines.insert(define_name);
					//continue; // comment the continue to keep the line. This will make GLSL also recognize the define.
				}

				size_t definesub_pos = line.find("#definesub ");
				if (definesub_pos != std::string::npos)
				{
					auto define_name_pos = line.find(" ", definesub_pos) + 1;
					auto define_name_end_pos = line.find(" ", define_name_pos) + 1;

					std::string define_name = line.substr(define_name_pos, define_name_end_pos - define_name_pos);
					std::string sub_name = line.substr(define_name_end_pos);

					rtrim(define_name);
					qtrim(define_name);

					rtrim(sub_name);
					qtrim(sub_name);

					define_subs.insert(std::pair<std::string, std::string>(define_name, sub_name));

					continue;
				}

				size_t undefinesub_pos = line.find("#undefinesub ");
				if (undefinesub_pos != std::string::npos)
				{
					auto define_name_pos = line.find(" ", undefinesub_pos) + 1;
					auto define_name_end_pos = line.find(" ", define_name_pos) + 1;

					std::string define_name = line.substr(define_name_pos, define_name_end_pos - define_name_pos);
					//std::string sub_name = line.substr(define_name_end_pos);

					rtrim(define_name);
					qtrim(define_name);

					//rtrim(sub_name);
					//qtrim(sub_name);

					//define_subs.insert(std::pair<std::string, std::string>(define_name, sub_name));
					define_subs.erase(define_name);

					continue;
				}

				size_t sub_pos = line.find("#sub ");
				if (sub_pos != std::string::npos)
				{
					auto define_name_pos = line.find(" ", sub_pos) + 1;
					auto define_name_end_pos = line.find(" ", define_name_pos) + 1;

					std::string define_name = line.substr(define_name_pos, define_name_end_pos - define_name_pos);

					rtrim(define_name);
					qtrim(define_name);

					auto sub_name_iter = define_subs.find(define_name);
					if (sub_name_iter != define_subs.end())
					{
						auto sub_name = sub_name_iter->second;

						line = line.erase(sub_pos, define_name_end_pos - sub_pos - 1); // -1 since there must be a preceeding space char
						line.insert(sub_pos, sub_name);
					}
					else continue;
				}


				complete_code.append(line);
				complete_code.append("\n");

				myfile.clear();
			}
			myfile.close();
		}
		else
		{
			std::cout << "ERROR: unable to open shader file: " << file_name << std::endl;
		}
	}

};

// ------------------------------------------------------------------------------------------------------------------------------------------


/*
**
*/
unsigned int renderer::shader::load_program(const std::vector<std::string>& files, const std::unordered_set<std::string>* defines_in, const std::unordered_map<std::string, std::string>* define_subs_in)
{
	
	std::unordered_set<std::string> defines = (defines_in == nullptr) ? std::unordered_set<std::string>() : *defines_in; // this will make a copy every call but it was fun to code this :D hope it works!! :D
	std::unordered_map<std::string, std::string> define_subs = (define_subs_in == nullptr) ? std::unordered_map<std::string, std::string>() : *define_subs_in; // this will make a copy every call but it was fun to code this :D hope it works!! :D
	

	unsigned int programId = glCreateProgram();
	unsigned int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	unsigned int geometryShaderId = glCreateShader(GL_GEOMETRY_SHADER);
	
		
	
	bool has_geometry_shader = false;
	
	std::vector<const char*> final_file_set;

	//final_file_set.push_back("assets/shaders/header.glsl");
	for (const std::string& str : files)
		final_file_set.push_back(str.c_str());

	///////////////////////////////////////////////////////////////////////////////
	// Vertex shader
	{
		defines.insert("VERTEX");
	
		std::vector<std::string*> final_code_set;
		for (unsigned int i = 0; i<final_file_set.size(); ++i)
		{
			std::string* complete_code = new std::string();
			load_file(*complete_code, final_file_set[i], &defines, &define_subs);
			final_code_set.push_back(complete_code);
		}

		std::vector<const char*> final_code_set_c;
		for (std::string* final_code : final_code_set)
			final_code_set_c.push_back(final_code->c_str());

		std::vector<int> final_code_set_len;
		for (std::string* final_code : final_code_set)
			final_code_set_len.push_back(static_cast<int>(final_code->size()));


		glShaderSource(vertexShaderId, static_cast<int>(final_code_set_c.size()), final_code_set_c.data(), final_code_set_len.data());
		glCompileShader(vertexShaderId);
		printShaderLog(vertexShaderId, final_code_set);

		defines.erase("VERTEX");

		for (std::string* complete_code : final_code_set)
			delete complete_code;
	}
	///////////////////////////////////////////////////////////////////////////////
	// Geometry shader
	{
		defines.insert("GEOMETRY");

		std::vector<std::string*> final_code_set;
		for (unsigned int i = 0; i<final_file_set.size(); ++i)
		{
			std::string* complete_code = new std::string();
			load_file(*complete_code, final_file_set[i], &defines, &define_subs);
			final_code_set.push_back(complete_code);
		}

		// see if we have a geometry shader. look for the main function
		has_geometry_shader = has_main_function(final_code_set);

		std::vector<const char*> final_code_set_c;
		for (std::string* final_code : final_code_set)
			final_code_set_c.push_back(final_code->c_str());

		std::vector<int> final_code_set_len;
		for (std::string* final_code : final_code_set)
			final_code_set_len.push_back(static_cast<int>(final_code->size()));


		glShaderSource(geometryShaderId, static_cast<int>(final_code_set_c.size()), final_code_set_c.data(), final_code_set_len.data());
		glCompileShader(geometryShaderId);
		printShaderLog(geometryShaderId, final_code_set);

		defines.erase("GEOMETRY");

		for (std::string* complete_code : final_code_set)
			delete complete_code;
	}
	///////////////////////////////////////////////////////////////////////////////
	// Fragment shader
	{
		defines.insert("FRAGMENT");

		std::vector<std::string*> final_code_set;
		for (unsigned int i = 0; i<final_file_set.size(); ++i)
		{
			std::string* complete_code = new std::string();
			load_file(*complete_code, final_file_set[i], &defines, &define_subs);
			final_code_set.push_back(complete_code);
		}

		std::vector<const char*> final_code_set_c;
		for (std::string* final_code : final_code_set)
			final_code_set_c.push_back(final_code->c_str());

		std::vector<int> final_code_set_len;
		for (std::string* final_code : final_code_set)
			final_code_set_len.push_back(static_cast<int>(final_code->size()));

		glShaderSource(fragmentShaderId, static_cast<int>(final_code_set_c.size()), final_code_set_c.data(), final_code_set_len.data());
		glCompileShader(fragmentShaderId);
		printShaderLog(fragmentShaderId, final_code_set);

		defines.erase("FRAGMENT");

		for (std::string* complete_code : final_code_set)
			delete complete_code;
	}


	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	if (has_geometry_shader) glAttachShader(programId, geometryShaderId);

	glLinkProgram(programId);
	return programId;
}

