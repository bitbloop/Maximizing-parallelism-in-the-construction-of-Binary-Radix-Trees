/******************************************************************************************

SHADER FILE FORMAT

A single shader file can contain code for the vertex, fragment, and geometry shaders.
The shader type belongs to is debined by cusom macros wtirren in the code:

#ifdef VERTEX
// ... vertex shader code
#endif

#ifdef FRAGMENT
// ... fragment shader code
#endif

#ifdef GEOMETRY
// ... geometry shader code
#endif

Code which is specified outside of the ifdef's is shared.

------------------------------------------------------------

You can also define values for the defines:
#definesub THISWILLBEREPLACED bythis

Now when the code encounters THISWILLBEREPLACED it will replace that string with bythis when used as:
#sub THISWILLBEREPLACED 
If the definesub is not set, the #sub ... will return an empty string. The #sub can be a block of code, and ot respects {}

You can remove string substitution that the define does with:
#undefinesub INOUT

A common use for this mechanism is to define the same interface for the vertex output and the fragment input:

#ifdef VERTEX
	#definesub INOUT out
#endif
#ifdef FRAGMENT
	#definesub INOUT in
#endif
#ifdef GEOMETRY
	#undefinesub INOUT
#endif

// output of the vertex shader, input to the fragment shader
#sub INOUT  Data {
	vec3 pos;
	vec2 uv;
} data_#sub INOUT ;

In this case, the vertex shader writes to data_out and the fragment shader reads from data_in.

------------------------------------------------------------

The include statements

#include filename

Will load the code from that file (respecting that file's include or definesub statements) and insert it into the place where the #include was.

------------------------------------------------------------

******************************************************************************************/


#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace renderer
{
	namespace shader
	{
		/** Returns a value acquired via glCreateProgram() */
		unsigned int load_program(const std::vector<std::string> &files, const std::unordered_set<std::string>* defines = nullptr, const std::unordered_map<std::string, std::string>* define_subs = nullptr);
	};
};