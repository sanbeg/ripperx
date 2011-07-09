#include <cstdlib>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include "../config.h"
#include "../src/misc_utils.h"
#include "../src/common.h"

using std::istream;
using std::ostream;
using std::copy;

_config config;

//read enums by casting to int
istream & operator >> (istream & in, CommonEncoderType &t) 
{
  return in >> (int&)t;
}


//interface for config entries
class ConfigRwDataBase 
{
public:
  friend istream & operator >> (istream & in, ConfigRwDataBase & e)
  {
    return e.operator >> (in);
  }
  
  friend ostream & operator << (ostream & out, ConfigRwDataBase const &e) 
  {
    return e.operator << (out);
  }
  
  virtual void to_default() = 0;
  void if_default() 
  {
    if (not flag) 
      {
	flag = true;
	to_default();
      }
  }
  void set_flag() 
  {
    flag = true;
  }
  
  ConfigRwDataBase() 
  {
    flag=false;
  }
  
private:
  virtual ostream & operator << (ostream & out) const = 0;
  virtual istream & operator >> (istream & in) = 0;
  bool flag;        //Flag is for error checking when reading config file
};
  

template <class T, class U>
struct ConfigRwData : public ConfigRwDataBase
{
private:
  T *m_id;         //pointer into config
  U default_value;

  ostream & operator << (ostream & out) const 
  {
    return out << *m_id;
  }
  
  istream & operator >> (istream & in) 
  {
    return in >> *m_id;
  }
  
public:
  void to_default() 
  {
    *m_id = default_value;
  }

  ConfigRwData(T * dst, const T & def) : default_value(def)
  {
    m_id = dst;
  }
};

//current c string handling is a little odd.
template <>
struct ConfigRwData <char, const char*> : public ConfigRwDataBase
{
private:
  char * m_id;         //pointer into config
  const char * default_value;
  ostream & operator << (ostream & out) const 
  {
    return out << m_id;
  }
  
  istream & operator >> (istream & in) 
  {
    //return in >> m_id;
    return in.getline(m_id, 1024, '\n');
  }
  
public:
  void to_default() 
  {
    copy (default_value, default_value+strlen(default_value)+1, m_id);
  }
  ConfigRwData(char * dst, const char * def) : default_value(def)
  {
    m_id = dst;
  }
};


typedef std::vector < std::pair<std::string,ConfigRwDataBase*> > config_rw_type;
config_rw_type config_rw_data;

template <class T, class U>
void insert_pair(const char * label, T * dst, const U dft) 
{
  
  config_rw_data.push_back(std::make_pair(label, new ConfigRwData<T,U>(dst,dft)));
}


void init_data (void) 
{
  insert_pair("General::WavRatio", &config.wav_ratio, 0.006f);
  insert_pair("General::Mp3Ratio", &config.mp3_ratio, 0.08f);
  insert_pair("General::ShellForExecution", config.shell_for_execution, "/bin/sh");
  insert_pair("General::WavPath", config.wav_path, "./");
  insert_pair("General::Mp3Path", config.wav_path, "./");
  insert_pair("General::CDDBPath", config.cddb_path,"./.cddbslave");
  
  
  insert_pair("General::WavFileNameFormat", config.wav_file_name_format,"track%");
  
  insert_pair(
	      "General::Mp3FileNameFormat", config.mp3_file_name_format,
	      "track%"
	      );
  insert_pair(
	      "General::PrependChar", &config.prepend_char, '_'
	      );
  insert_pair(
	      "General::MakeMp3FromExistingWav", 
	      &config.make_mp3_from_existing_wav,0
	      );
  insert_pair(
	      "General::AskWhenFileExists", &config.ask_when_file_exists, 1
	      );
  insert_pair(
	      "General::AutoAppendExtension", 
	      &config.auto_append_extension, 1
	      );
  insert_pair(
	      "General::KeepWav", &config.keep_wav, 0
	      );
  insert_pair(
	      "Ripper::Ripper", config.ripper.ripper,	
	      "cdparanoia                "
	      );
  insert_pair(
	      "Ripper::Plugin", config.ripper.plugin,
	      "ripperX_plugin-cdparanoia"
	      );
  insert_pair(
	      "Encoder::Encoder", config.encoder.encoder,
	      "lame"
	      );
  insert_pair(
	      "Encoder::Type", &config.encoder.type,MP3
	      );
  insert_pair(
	      "Encoder::Bitrate", &config.encoder.bitrate,128
	      );
  insert_pair(
	      "Encoder::VarBitrate", &config.encoder.use_varbitrate,1
	      );
  insert_pair(
	      "Encoder::VBRQual", &config.encoder.vbr_qual,4
	      );
  insert_pair(
	      "Encoder::Priority", &config.encoder.priority,10
	      );
  insert_pair(
	      "Encoder::HighQual", &config.encoder.use_high_qual,1
	      );
  insert_pair(
	      "Encoder::useCRC", &config.encoder.use_crc,0
	      );
  insert_pair(
	      "Encoder::extraOptions", config.encoder.extra_options,""
	      );
  insert_pair(
	      "Encoder::fullCommand", config.encoder.full_command,
	      "lame -b 128"
	      );
  insert_pair(
	      "Encoder::Plugin", config.encoder.plugin,
	      "ripperX_plugin-lame"
	      );
  insert_pair(
	      "CdPlayer::Play_command", config.cd_player.play_command,
	      "cdplay %"
	      );
  insert_pair(
	      "CdPlayer::Stop_command", config.cd_player.stop_command,
	      "cdstop"
	      );
  insert_pair(
	      "WavPlayer::Command", config.wav_player.command,
	      "play %"
	      );
  insert_pair(
	      "Mp3Player::Command", config.mp3_player.command,
	      "mpg123 %"
	      );
  insert_pair(
	      "CDDBConfig::Server", config.cddb_config.server,
	      "freedb.freedb.org/~cddb/cddb.cgi"
	      );
  insert_pair(
	      "CDDBConfig::Port", &config.cddb_config.port,80
	      );
  insert_pair(
	      "CDDBConfig::UseHttp", &config.cddb_config.use_http,1
	      );
  insert_pair(
	      "CDDBConfig::ProxyServer", config.cddb_config.proxy_server,
	      ""
	      );
  insert_pair(
	      "CDDBConfig::ProxyPort", &config.cddb_config.proxy_port,8080
	      );
  insert_pair(
	      "CDDBConfig::ConvertSpaces", &config.cddb_config.convert_spaces,0
	      );
  insert_pair(
	      "CDDBConfig::MakeDirectories", 
	      &config.cddb_config.make_directories,1
	      );
  insert_pair(
	      "CDDBConfig::CreateID3", &config.cddb_config.create_id3,1
	      );
  insert_pair(
	      "CDDBConfig::CreatePlaylist", 
	      &config.cddb_config.create_playlist,1
	      );
  insert_pair(
	      "CDDBConfig::AutoLookup", &config.cddb_config.auto_lookup,0
	      );
  insert_pair(
	      "CDDBConfig::FormatString", config.cddb_config.format_string,
	      "%a - %s"
	      );
  insert_pair(
	      "CDDBConfig::DirFormatString", 
	      config.cddb_config.dir_format_string,
	      "%a - %v"
	      );
  sort(config_rw_data.begin(),config_rw_data.end());
}

void write_config(void)
{
  using std::ofstream;
  using std::endl;
  
  //ofstream file(construct_file_name(std::getenv("HOME"), ".ripperXrc"));
  ofstream file ("test.ripperXrc");
  
  //check error, see if we should create, etc...
  file << "//\n";
  file << "// ~/.ripperXrc\n";
  file << "// This is the resource file for ripperX.\n";
  file << "// If you edit this file with an editor, you must leave all\n";
  file << "// parameters in the order in which they appear.  Also note\n";
  file << "// that this file is overwritten each time ripperX is run.\n";
  file << "//\n// You can configure everything in the config menu within ripperX.\n";
  file << "//\n\n";
  file << "//-v " << VERSION << "\n\n";
  for (config_rw_type::iterator it=config_rw_data.begin(); 
       it != config_rw_data.end(); 
       ++it) 
    {
      it->second->if_default();
      
      //
      file << it->first << " = " << *it->second << std::endl;
    }
  
}

void read_config(void)
{
  using std::cout;
  using std::endl;
  using std::string;

  //std::ifstream file(construct_file_name(getenv("HOME"), ".ripperXrc"));
  std::ifstream file(".ripperXrc");

  //each non-comment line is label = value \n
  string label, eq, value;
  ConfigRwDataBase *dum_ptr=0;
  
  while (file >> label) 
    {
      if (not label.compare(0, 2, "//")) 
	{
	  file.ignore(128,'\n');
	  continue;
	}
      
      if (!(file >> eq) or eq.compare("="))
	throw std::runtime_error("error parsing config file");

      config_rw_type::iterator entry = lower_bound(
						   config_rw_data.begin(),
						   config_rw_data.end(),
						   std::make_pair(label,dum_ptr));
      if (entry->first == label) 
	{
	  if (file >> *(entry->second))
	    entry->second->set_flag();
	}
      else 
	throw std::runtime_error(string("Invalid config entry:")+label);
    }
}


int main (void) 
{
  init_data();
  read_config();
  write_config();
}
