// Copyright 2017 Edward G. Bruck <ed.bruck1@gmail.com>
//
// This file is part of Radiotray-NG.
//
// Radiotray-NG is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Radiotray-NG is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Radiotray-NG.  If not, see <http://www.gnu.org/licenses/>.

#include <radiotray-ng/config/config.hpp>
#include <radiotray-ng/helpers.hpp>

#include <fstream>
#include <cstring>


Config::Config(const std::string& config_file)
	: config_file(radiotray_ng::word_expand(config_file))
{
}


bool Config::save()
{
	std::lock_guard<std::mutex> lock(this->config_lock);

	LOG(debug) << "saving: " << this->config_file;

	try
	{
		std::ofstream ofile(this->config_file);
		ofile.exceptions(std::ios::failbit);

		ofile << Json::StyledWriter().write(this->config);
	}
	catch(std::ios_base::failure& /*e*/)
	{
		LOG(error) << "Failed to save: " << this->config_file << " : "<< strerror(errno);
		return false;
	}

	return true;
}


bool Config::load()
{
	std::lock_guard<std::mutex> lock(this->config_lock);

	LOG(debug) << "loading: " << this->config_file;

	try
	{
		std::ifstream ifile(this->config_file);
		ifile.exceptions(std::ios::failbit);

		Json::Reader reader;
		if (!reader.parse(ifile, this->config))
		{
			LOG(error) << "Failed to parse: " << this->config_file << " : " << reader.getFormattedErrorMessages();
			return false;
		}
	}
	catch(std::ios_base::failure& /*e*/)
	{
		LOG(error) << "Failed to load: " << this->config_file << " : "<< strerror(errno);
		return false;
	}

	return true;
}


template<typename T>
void Config::private_set_value(const std::string& key, const T& value)
{
	std::lock_guard<std::mutex> lock(this->config_lock);

	this->config[key] = value;
}


void Config::set_string(const std::string& key, const std::string& value)
{
	this->private_set_value(key, value);
}


void Config::set_uint32(const std::string& key, const uint32_t value)
{
	this->private_set_value(key, value);
}


void Config::set_bool(const std::string& key, const bool value)
{
	this->private_set_value(key, value);
}


std::string Config::get_string(const std::string& key, const std::string& default_value)
{
	std::lock_guard<std::mutex> lock(this->config_lock);

	if (this->config.isMember(key))
	{
		return this->config[key].asString();
	}

	return default_value;
}


uint32_t Config::get_uint32(const std::string& key, const uint32_t default_value)
{
	std::lock_guard<std::mutex> lock(this->config_lock);

	if (this->config.isMember(key))
	{
		return this->config[key].asUInt();
	}

	return default_value;
}


bool Config::get_bool(const std::string& key, const bool default_value)
{
	std::lock_guard<std::mutex> lock(this->config_lock);

	if (this->config.isMember(key))
	{
		return this->config[key].asBool();
	}

	return default_value;
}