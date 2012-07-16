// Soar-SC-No-Dependencies.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <utility>

#include "sml_Client.h"

const std::string unit_box_verts = "0 0 0 0 0 1 0 1 0 0 1 1 1 0 0 1 0 1 1 1 0 1 1 1";

bool verbose;

bool is_number(std::string number)
{
	using namespace std;

	bool is_number = !number.empty() && number.find_first_not_of("-.0123456789") == string::npos;
	if (!is_number)
		return false;

	size_t pos_of_negative = number.find("-");

	if (pos_of_negative != string::npos && pos_of_negative != 0)
		return false;

	size_t period_pos = number.find(".");

	if (period_pos != string::npos && number.find(".", period_pos+1) != string::npos)
		return false;

	return true;
}

bool is_float(std::string number)
{
	using namespace std;

	if (!is_number(number))
		return false;

	size_t period_pos = number.find(".");

	if (period_pos == string::npos)
		return false;

	if (period_pos != string::npos && number.find(".", period_pos+1) != string::npos)
		return false;
	
	return true;
}

bool parse_line(std::string line, std::vector<std::vector<std::string> > &strings)
{
	using namespace std;

	vector<string> line_vector;

	istringstream iss(line, istringstream::in);
	
	string word;
	while (iss >> word)
		line_vector.push_back(word);

	strings.push_back(line_vector);

	return true;
}

bool parse_command(std::vector<std::string> &command, sml::Agent *agent)
{
	using namespace std;
	using namespace sml;

	if (command.size() < 3)
	{
		cerr << "Command must be in the format of 'Type-identifier-...-identifier: ^param value'" << endl;
		return false;
	}

	string command_type = command[0];

	if (command_type.find(':') == string::npos)
	{
		cerr << "Could not find ':'.  This (" << command_type << ") is an invalid command!" << endl;
		return false;
	}

	if (command_type.size() < 4)
	{
		cerr << "Command type is invalid.  It is less than four characters, minimum is I-C: or SVS: both are four characters! '" << command_type << "'" << endl;
		return false;
	}

	Identifier* input_link = agent->GetInputLink();

	if (command_type[0] == 'I')
	{
		if (command_type[1] != '-')
		{
			cerr << "Command for Input Link must be in the format of I-<C>: where <C> is one or more characters: '" << command_type << "'" << endl;
			return false;
		}

		string location(command_type.begin()+2, command_type.end()-1);

		vector<string> locations;

		string location_sub;
		
		int previous_pos = 0;
		while (true)
		{
			int pos = location.find("-", previous_pos);

			if (pos == string::npos)
			{
				locations.push_back(string(location.begin()+previous_pos, location.end()));
				break;
			}
			else
			{
				locations.push_back(string(location.begin()+previous_pos, location.begin()+pos));
				previous_pos = pos+1;
			}
		}

		Identifier* location_id = input_link;
		
		for (vector<string>::iterator it = locations.begin();it != locations.end();it++)
		{
			Identifier* new_location_id = NULL;

			for (int i = 0;i < location_id->GetNumberChildren();i++)
			{
				WMElement* potential_id = location_id->FindByAttribute(it->c_str(), i);
				if (potential_id && potential_id->IsIdentifier())
				{
					new_location_id = potential_id->ConvertToIdentifier();
					break;
				}
			}

			if (!new_location_id)
			{
				if (verbose)
					cout << "Creating id '" << *it << "' on the input link" << endl;

				new_location_id = location_id->CreateIdWME(it->c_str());
			}
			else if (verbose)
				cout << "Using existing id '" << *it << "' on the input link" << endl;

			if (!new_location_id)
			{
				cerr << "Could not create: '" << *it << "' on input link!" << endl;
				return false;
			}

			location_id = new_location_id;
		}

		if ((command.size()-1)%2 != 0)
		{
			cerr << "Commands must be in pairs!" << endl;
			return false;
		}

		vector<pair<string,string> > params;
		for (vector<string>::iterator it = command.begin()+1;it != command.end();it += 2)
		{
			if (it->at(0) != '^')
			{
				cerr << "Invalid parameter: '" << *it << "'.  Must start with ^ (caret)" << endl;
				return false;
			}

			string parameter(it->begin()+1, it->end());
			string value((it+1)->begin(), (it+1)->end());

			bool is_a_number = is_number(value);
			bool is_a_float = is_float(value);

			WMElement* element;

			if (is_a_number)
			{
				float command_num = 0;

				stringstream ss(value);
				ss >> command_num;

				if (is_a_float)
				{
					if (verbose)
						cout << "Creating float WME on the input link: '^" << parameter << " " << value << "'" << endl;

					element = location_id->CreateFloatWME(parameter.c_str(), command_num);
				}
				else
				{
					if (verbose)
						cout << "Creating int WME on the input link: '^" << parameter << " " << value << "'" << endl;
					
					element = location_id->CreateIntWME(parameter.c_str(), static_cast<int>(command_num));
				}
			}
			else
			{
				if (verbose)
						cout << "Creating string WME on the input link: '^" << parameter << " " << value << "'" << endl;
				
				element = location_id->CreateStringWME(parameter.c_str(), value.c_str());
			}

			if (!element)
			{
				cerr << "Unable to create WME of type ";
				if (is_a_number && is_a_float)
					cerr << "float";
				else if (is_a_number)
					cerr << "int";
				else
					cerr << "string";
				
				cerr << " with a value of '" << value << "'" << endl;
				return false;
			}
		}

	}
	else if (command_type == "SVS:")
	{
		if ((command.size()-1) > 4)
		{
			cerr << "SVS can only accept a total of 4 inputs, ID, Position, Rotation, and Scale!" << endl;
			return false;
		}

		string svs_command = "a ";
		
		int pos = 0;
		for (vector<string>::iterator it = command.begin()+1;it != command.end();it++)
		{
			pos++;

			if (pos == 0)
			{
				svs_command += *it;
				svs_command += " world v " + unit_box_verts;
				continue;
			}

			int comma_pos = 0;
			if ((comma_pos = it->find(",")) == string::npos)
			{
				cerr << "SVS Input cannot be 1 dimensional: '" << *it << "'" << endl;
				return false;
			}

			string first_half(it->begin(), it->begin()+comma_pos);
			string second_half;
			
			if (it->find(",") == string::npos)
				second_half = string(it->begin()+comma_pos+1, it->end());
			else
			{
				cerr << "SVS Input cannot be more than 2 dimensional input! Requested at least 3: '" << *it << "'" << endl;
				return false;
			}

			if (!is_number(first_half))
			{
				cerr << "SVS Input must be numbers! '" << first_half << "'" << endl;
				return false;
			}
			else if (!is_number(second_half))
			{
				cerr << "SVS Input must be numbers! '" << second_half << "'" << endl;
				return false;
			}

			if (pos == 1)
				svs_command += " p ";
			else if (pos == 2)
				svs_command += " r ";
			else
				svs_command += " s ";

			svs_command += first_half + " " + second_half + (pos > 2 ? " 1" : " 0");
		}

		if (verbose)
			cout << "SVS Command: '" << svs_command << "'" << endl;

		agent->SendSVSInput(svs_command);
	}
	else if (command_type == "SVS-Actual:")
	{
		string svs_command;

		for (vector<string>::iterator it = command.begin()+1;it != command.end();it++)
			svs_command += " " + *it;

		if (verbose)
			cout << "SVS Command: '" << svs_command << "'" << endl;

		agent->SendSVSInput(svs_command);
	}
	else
	{
		cerr << "Invalid command: '" << command_type << "'.  Must be either I-C: (minimum) or SVS: or SVS-Actual:" << endl;
		return false;
	}

	return true;
}

int main(int argc, char* argv[])
{
	using namespace std;
	using namespace sml;

	verbose = false;

	if (argc > 3 || argc < 2)
	{
		cout << "Too many arguments, " << argc << endl;
		cout << argv[0] << " <file> <verbose=no>" << endl;
		cout << "<file> can be an absolute or relative path." << endl;
		cout << "<verbose> is whether to be verbose about printing.  Accepted values are (without quotes): 'yes', 'no', 'true', 'false', '1', '0' where 1 is true/yes and 0 is false/no." << endl;

		return 1;
	}
	
	string file_path = argv[1];

	if (argc == 3)
	{
		string argument = argv[2];

		if (argument == "yes" || argument == "true" || argument == "1")
			verbose = true;
		else if (argument == "no" || argument == "false" || argument == "0")
			verbose = false;
		else
		{
			cout << argv[0] << " <file> <verbose=no>" << endl;
			cout << "<file> can be an absolute or relative path." << endl;
			cout << "<verbose> is whether to be verbose about printing.  Accepted values are (without quotes): 'yes', 'no', 'true', 'false', '1', '0' where 1 is true/yes and 0 is false/no." << endl;

			return 2;
		}
	}

	//Reads a file into memory
	//Then interprets it and sends the correct Soar Commands (Note the spaces!):
	//I-Units: unit ^param value ^param value ...
	//SVS: ID X,Y
	//SVS-Actual:<Actual SVS Command>
	//For SVS-Actual: anything after the colon ':' will be considered part of the SVS command!
	//For SVS the order is position, rotation (in radians!), scale for the number pairs except for ID which is a string

	if (verbose)
		cout << "Opening file '" << file_path << "'" << endl;

	ifstream file(file_path, ios::in);

	if (!file.is_open())
	{
		cerr << "Could not open the test file: '" << file_path << "'" << endl;
		return 3;
	}

	vector<vector<string> > lines;

	string line;
	while (getline(file, line))
	{
		if (!parse_line(line, lines))
		{
			cerr << "Error parsing line: '" << line << "'" << endl;
			return 4;
		}
	}

	if (verbose)
		cout << "Done parsing file.  Now sending to Soar" << endl;

	Kernel* kernel = Kernel::CreateKernelInNewThread();
	Agent* agent = kernel->CreateAgent("Soar-SC");

	for (vector<vector<string> >::iterator it = lines.begin();it != lines.end();it++)
	{
		if (!parse_command((*it), agent))
		{
			cerr << "Error parsing command: '";
			for (vector<string>::iterator jt = it->begin();jt != it->end();jt++)
				cerr << (*jt) << " ";

			cerr << endl;

			return 5;
		}
	}

	if (verbose)
		cout << "Done sending input to Soar.  Executing command: 'run -d 1'" << endl;

	agent->ExecuteCommandLine("run -d 1");
	
	cout << "Done." << endl;

	return 0;
}

