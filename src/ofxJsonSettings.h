//
//  ofJsonSettings.h
//  ofJsonSettings
//
//  Created by Matt Felsen on 12/7/15.
//
//

#pragma once
#include "ofMain.h"

// helper for shorter syntax
class ofxJsonSettings;
typedef ofxJsonSettings Settings;

class ofxJsonSettings {
public:

	// Singleton..
	static ofxJsonSettings& get() {
		static ofxJsonSettings instance;
		return instance;
	}

	void setDelimiter(const std::string & delim);
	bool load(const std::string & file = "settings.json");
	bool save(const std::string & file = "settings.json", bool prettyPrint = true);

	string getAsJsonString(); //in case you want to print / read

	// Use this to get values
	// These return references, so you can bind directly to a GUI library that
	// supports variable binding
	static std::string& getString(const std::string & key);
	static bool& getBool(const std::string & key);
	static int& getInt(const std::string & key);
	static float& getFloat(const std::string & key);
	static double& getDouble(const std::string & key);
	static ofVec2f& getVec2(const std::string & key);
	static ofVec3f& getVec3(const std::string & key);
	static ofVec4f& getVec4(const std::string & key);
	static ofColor& getColor(const std::string & key);
	static ofJson getJson(const std::string & key);

	// Use this to check if a key exists
	static bool exists(const std::string & key);

	static bool remove(const std::string & key);

	ofEvent<void> settingsLoaded;
	ofEvent<void> settingsSaved;

protected:
	string& _stringVal(const std::string& key);
	bool& _boolVal(const std::string& key);
	int& _intVal(const std::string& key);
	float& _floatVal(const std::string& key);
	double& _doubleVal(const std::string& key);
	ofVec2f& _vec2Val(const std::string& key);
	ofVec3f& _vec3Val(const std::string& key);
	ofVec4f& _vec4Val(const std::string& key);
	ofColor& _colorVal(const std::string& key);
	ofJson _jsonVal(const std::string& key);

	string _stringValFromJson(ofJson& data, const std::string & key);
	bool _boolValFromJson(ofJson& data, const std::string & key);
	int _intValFromJson(ofJson& data, const std::string & key);
	float _floatValFromJson(ofJson& data, const std::string & key);
	double _doubleValFromJson(ofJson& data, const std::string & key);
	ofVec2f _vec2ValFromJson(ofJson& data, const std::string & key);
	ofVec3f _vec3ValFromJson(ofJson& data, const std::string & key);
	ofVec4f _vec4ValFromJson(ofJson& data, const std::string & key);
	ofColor _colorValFromJson(ofJson& data, const std::string & key);

	bool _exists(const std::string & key);
	bool _remove(const std::string & key);

	ofJson getNestedChild(ofJson & data, const std::string & key, bool supressWarning = false);
	ofJson getNestedChild(ofJson & data, vector<std::string> & keys, bool supressWarning = false);

	template<typename T>
	void setNestedChild(ofJson& data, const std::string & key, T val);

	template<typename T>
	void setNestedChild(ofJson& data, vector<std::string> & keys, T val);

	template<typename T>
	void cacheToJson(T& container, ofJson& data);

	void cacheToJson(std::unordered_map<std::string,ofVec2f>& container, ofJson& data);
	void cacheToJson(std::unordered_map<std::string,ofVec3f>& container, ofJson& data);
	void cacheToJson(std::unordered_map<std::string,ofVec4f>& container, ofJson& data);
	void cacheToJson(std::unordered_map<std::string,ofColor>& container, ofJson& data);

	template<typename T>
	bool exists(T& container, const std::string &key);

	template<typename T>
	void printMap(T& container, const std::string& text = "");

	ofJson jsonStore;
	std::unordered_map<std::string,std::string> stringMap;
	std::unordered_map<std::string,int> intMap;
	std::unordered_map<std::string,bool> boolMap;
	std::unordered_map<std::string,float> floatMap;
	std::unordered_map<std::string,double> doubleMap;
	std::unordered_map<std::string,ofVec2f> vec2Map;
	std::unordered_map<std::string,ofVec3f> vec3Map;
	std::unordered_map<std::string,ofVec4f> vec4Map;
	std::unordered_map<std::string,ofColor> colorMap;

	string delimiter;

private:

	ofxJsonSettings();
	// make sure there are no copies
	ofxJsonSettings(ofxJsonSettings const&);
	void operator=(ofxJsonSettings const&);
};
