/*
 * This file was automaticly generated by clo++ version 0.6.3 (REL-0_6_3)
 * http://pmade.org/pjones/software/clo++/
 */

/** @file
 * This file contains the implementation of the parser class.
 * You can control the name of this source file using the
 * cxx_source_file variable.
**/

#include "clo.h"
#include <cstring>
#include <cstdlib>
#include <cctype>

namespace {
    const char const_usage[] =
"  --builtin          Use the display mode's builtin shader, if one exists.\n"
"  -l, --lut string   Load a display LUT from the specified file.\n"
"  -p, --usepdr       Use NVIDIA Pixel Data Range extension.\n"
"  -r, --rate double  Frame rate of sequence (default is 24.0).\n"
"  -v, --version      Print the version and then exit.\n";

    const char const_help_comment[] =
"use the -h option for help";

    const char* expand_long_name (const std::string &name);
}

//#########################################################################
clo::parser::parser (void)
    : state_(state_option)
{
    std::memset(&locations_, 0, sizeof(locations_));
}
//#########################################################################
void clo::parser::parse (int argc, char *argv[], bool call_finalize) {
    for (int i=1; i<argc; ++i) parse_element(argv[i], i, source_cl);
    if (call_finalize) finalize();
}
//#########################################################################
void clo::parser::finalize (void) {
    if (state_ == state_value) {
	switch (openum_) {
	    case option_builtin:
		throw option_error("missing value for 'builtin' option");
	    case option_lut:
		throw option_error("missing value for 'lut' option");
	    case option_rate:
		throw option_error("missing value for 'rate' option");
	    case option_usePdr:
		throw option_error("missing value for 'usepdr' option");
	    case option_version:
		throw option_error("missing value for 'version' option");
	}

    }

}
//#########################################################################
void clo::parser::parse_element (const char *element, int position, opsource source) {
    std::size_t length = std::strlen(element);

    switch (state_) {
	case state_consume:
	    non_options_.push_back(element);
	    break;
	case state_option:
	    if (length >= 2 && element[0] == '-' && element[1] == '-') {
		if (length == 2) { state_ = state_consume; return; }
		element += 2;
		const char *value = element;
		while (*value != 0 && *value != '=') ++value;
		if (*value == '=') {
		    std::string selement(element, value - element), svalue(++value);

		    parse_long_option(selement.c_str(), position, source);
		    if (state_ != state_value) {
			std::string error("the '"); error += element; error += "' option does not take a value";
			throw option_error(error);
		    }

		    parse_value(svalue.c_str());
		    state_ = state_option;
		} else {
		    parse_long_option(element, position, source);
		}
	    } else if (length >= 2 && element[0] == '-') {
		++element;

		if (length > 2) {
		    while (*element != 0) {
			parse_short_option(*element, position, source);
			++element;
	    
			if (state_ == state_value && *element != 0) {
			    parse_value(element);
			    state_ = state_option;
			    break;
			}
		    }
		} else {
		    parse_short_option(*element, position, source);
		}
	    } else {
		non_options_.push_back(element);
	    }
	    break;
	case state_value:
	    parse_value(element);
	    state_ = state_option;
	    break;
    }
}
//#########################################################################
void clo::parser::parse_short_option (char option, int position, opsource source) {
    switch (option) {
    	case 'l':
    	    openum_ = option_lut;
    	    state_ = state_value;
    	    locations_.lut = position;
    	    return;
    	case 'p':
    	    openum_ = option_usePdr;
    	    options_.usePdr = !options_.usePdr;
    	    locations_.usePdr = position;
    	    return;
    	case 'r':
    	    openum_ = option_rate;
    	    state_ = state_value;
    	    locations_.rate = position;
    	    return;
    	case 'v':
    	    throw autoexcept(autothrow_version, "version");
    	case 'h':
    	    if (source != source_cl) break;
    	    throw autoexcept(autothrow_help, const_usage);
    	case '?':
    	    if (source != source_cl) break;
    	    throw autoexcept(autothrow_help, const_usage);
    }

    std::string error("unknown option: '"); error += option; error += "'";
    throw option_error(error);
}
//#########################################################################
void clo::parser::parse_long_option (const char *option, int position, opsource source) {
    option = expand_long_name(option);

	    if (std::strcmp(option, "builtin") == 0) {
		openum_ = option_builtin;
		locations_.builtin = position;
		options_.builtin = !options_.builtin;
		return;
	    } else if (std::strcmp(option, "lut") == 0) {
		openum_ = option_lut;
		locations_.lut = position;
		state_ = state_value;
		return;
	    } else if (std::strcmp(option, "rate") == 0) {
		openum_ = option_rate;
		locations_.rate = position;
		state_ = state_value;
		return;
	    } else if (std::strcmp(option, "usepdr") == 0) {
		openum_ = option_usePdr;
		locations_.usePdr = position;
		options_.usePdr = !options_.usePdr;
		return;
	    } else if (std::strcmp(option, "version") == 0) {
		throw autoexcept(autothrow_version, "version");
	    } else if (source == source_cl && std::strcmp(option, "help") == 0) {
		throw autoexcept(autothrow_help, const_usage);
	    }

    std::string error("unknown option '"); error += option; error += "'";
    throw option_error(error);
}
//#########################################################################
void clo::parser::parse_value (const char *value) {
    switch (openum_) {
    	case option_builtin:
    	    break;
    	case option_lut:
    	    {
    		options_.lut = value;
    	    }
    	    break;
    	case option_rate:
    	    {
    		char *endptr; double tmp = std::strtod(value, &endptr);
    		while (*endptr != 0 && std::isspace(*endptr)) ++endptr;

    		if (*endptr != 0) {
    		    std::string error("invalid floating point value '"); error += value; error += "'";
    		    throw option_error(error);
    		}

    		if (tmp < 0) {
    		    throw option_error("floating point value out of range, 'rate' min is 0");
    		}

    		options_.rate = tmp;
    	    }
    	    break;
    	case option_usePdr:
    	    break;
    	case option_version:
    	    break;
    }
}
//#########################################################################
const char* clo::option_error::get_help_comment (void) const {
    return const_help_comment;
}
//#########################################################################
namespace {
    const char* expand_long_name (const std::string &name) {
	std::string::size_type name_size = name.size();
	std::vector<const char*> matches;

        if (name_size <= 7 && name.compare(0, name_size, "builtin", name_size) == 0)
        	matches.push_back("builtin");

        if (name_size <= 3 && name.compare(0, name_size, "lut", name_size) == 0)
        	matches.push_back("lut");

        if (name_size <= 4 && name.compare(0, name_size, "rate", name_size) == 0)
        	matches.push_back("rate");

        if (name_size <= 6 && name.compare(0, name_size, "usepdr", name_size) == 0)
        	matches.push_back("usepdr");

        if (name_size <= 7 && name.compare(0, name_size, "version", name_size) == 0)
        	matches.push_back("version");

        if (name_size <= 4 && name.compare(0, name_size, "help", name_size) == 0)
        	matches.push_back("help");


	if (matches.empty()) {
	    std::string error("unknown option '"); error += name; error += "'";
	    throw clo::option_error(error);
	}

	if (matches.size() == 1) {
	    return matches[0];
	}

	std::string error("the option name '"); error += name; error += "' is ambiguous";
	throw clo::option_error(error);
    }
} // end anonymous namespace