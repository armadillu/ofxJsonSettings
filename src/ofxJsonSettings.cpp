//
//  ofxJsonSettings.cpp
//  ofxJsonSettings
//
//  Created by Matt Felsen on 12/10/15.
//
//

#include "ofxJsonSettings.h"

using namespace std;

ofxJsonSettings::ofxJsonSettings() {
	delimiter = "/";
}

void ofxJsonSettings::setDelimiter(const string & delim) {
	delimiter = delim;
}

inline ofJson myLoadJson(const of::filesystem::path & filename){
	ofJson json;
	ofFile jsonFile(filename);
	if(jsonFile.exists()){
		try{
			json = nlohmann::json::parse(jsonFile,
									 /* callback */ nullptr,
									 /* allow exceptions */ true,
									 /* ignore_comments */ true);
		}catch(std::exception & e){
			ofLogError("ofLoadJson") << "Error loading json from " << filename << ": " << e.what();
		}catch(...){
			ofLogError("ofLoadJson") << "Error loading json from " << filename;
		}
	}else{
		ofLogError("ofLoadJson") << "Error loading json from " << filename << ": file doesn't exist";
	}
	return json;
}


bool ofxJsonSettings::load(const string & file) {
	ofJson loadData = myLoadJson(file);
	bool success = !loadData.empty();

	if (success) {
		jsonStore = loadData;

		// Any values bound to gui are bound to memory address of value
		// inside various cached map objects, so we can't clear() the maps
		// Instead, load new values from new json object to cached maps

		// TODO: remove old keys when loading
		// Any keys that were in the old json that have been removed from
		// the new file won't be deleted from the cached maps. Need to compare
		// jsonStore vs loadData objects and remove missing keys?
		for (auto& it : stringMap) {
			string key = it.first;
			stringMap[key] = _stringValFromJson(jsonStore, key);
		}
		for (auto& it : boolMap) {
			string key = it.first;
			boolMap[key] = _boolValFromJson(jsonStore, key);
		}
		for (auto& it : intMap) {
			string key = it.first;
			intMap[key] = _intValFromJson(jsonStore, key);
		}
		for (auto& it : floatMap) {
			string key = it.first;
			floatMap[key] = _floatValFromJson(jsonStore, key);
		}
		for (auto& it : doubleMap) {
			string key = it.first;
			doubleMap[key] = _doubleValFromJson(jsonStore, key);
		}
		for (auto& it : vec2Map) {
			string key = it.first;
			vec2Map[key] = _vec2ValFromJson(jsonStore, key);
		}
		for (auto& it : vec3Map) {
			string key = it.first;
			vec3Map[key] = _vec3ValFromJson(jsonStore, key);
		}
		for (auto& it : vec4Map) {
			string key = it.first;
			vec4Map[key] = _vec4ValFromJson(jsonStore, key);
		}
		for (auto& it : colorMap) {
			string key = it.first;
			colorMap[key] = _colorValFromJson(jsonStore, key);
		}

		ofNotifyEvent(settingsLoaded);
	} else {
		ofLogError("Settings") << "could not load file! " << file;
	}

	return success;
}

bool saveJsonPretty(const of::filesystem::path & filename, const ofJson & json){
	ofFile jsonFile(filename, ofFile::WriteOnly);
	try{
		jsonFile << json.dump(1, '\t');
	}catch(std::exception & e){
		ofLogError("ofSavePrettyJson") << "Error saving json to " << filename << ": " << e.what();
		return false;
	}catch(...){
		ofLogError("ofSavePrettyJson") << "Error saving json to " << filename;
		return false;
	}
	return true;
}

bool ofxJsonSettings::save(const string & file, bool prettyPrint) {
	// Write cached values back to JSON object
	cacheToJson(stringMap, jsonStore);
	cacheToJson(boolMap, jsonStore);
	cacheToJson(intMap, jsonStore);
	cacheToJson(floatMap, jsonStore);
	cacheToJson(doubleMap, jsonStore);
	cacheToJson(vec2Map, jsonStore);
	cacheToJson(vec3Map, jsonStore);
	cacheToJson(vec4Map, jsonStore);
	cacheToJson(colorMap, jsonStore);
	bool success = saveJsonPretty(file, prettyPrint);

	if (success) {
		ofNotifyEvent(settingsSaved);
	} else {
		ofLogError("Settings") << "could not save file! " << file;
	}

	return success;
}

string ofxJsonSettings::getAsJsonString(){

	ofJson tempJson = jsonStore;
	cacheToJson(stringMap, tempJson);
	cacheToJson(boolMap, tempJson);
	cacheToJson(intMap, tempJson);
	cacheToJson(floatMap, tempJson);
	cacheToJson(doubleMap, tempJson);
	cacheToJson(vec2Map, tempJson);
	cacheToJson(vec3Map, tempJson);
	cacheToJson(vec4Map, tempJson);
	cacheToJson(colorMap, tempJson);
	string pretty = tempJson.dump(1, '\t');
	return pretty;
}

string& ofxJsonSettings::getString(const string & key){
	return get()._stringVal(key);
}
bool& ofxJsonSettings::getBool(const string & key){
	return get()._boolVal(key);
}
int& ofxJsonSettings::getInt(const string & key){
	return get()._intVal(key);
}
float& ofxJsonSettings::getFloat(const string & key){
	return get()._floatVal(key);
}
double& ofxJsonSettings::getDouble(const string & key){
	return get()._doubleVal(key);
}
ofVec2f& ofxJsonSettings::getVec2(const string & key){
	return get()._vec2Val(key);
}
ofVec3f& ofxJsonSettings::getVec3(const string & key){
	return get()._vec3Val(key);
}
ofVec4f& ofxJsonSettings::getVec4(const string & key){
	return get()._vec4Val(key);
}
ofColor& ofxJsonSettings::getColor(const string & key){
	return get()._colorVal(key);
}
ofJson ofxJsonSettings::getJson(const string & key){
	return get()._jsonVal(key);
}

bool ofxJsonSettings::exists(const string & key){
	return get()._exists(key);
}

bool ofxJsonSettings::remove(const string & key){
	return get()._remove(key);
}

string& ofxJsonSettings::_stringVal(const string& key) {
	if (!exists(stringMap, key)) {
		stringMap[key] = _stringValFromJson(jsonStore, key);
	}

//    printMap(stringMap, "strings");
	return stringMap[key];
}

bool& ofxJsonSettings::_boolVal(const string& key) {
	if (!exists(boolMap, key)) {
		boolMap[key] = _boolValFromJson(jsonStore, key);
	}

//    printMap(boolMap, "bools");
	return boolMap[key];
}

int& ofxJsonSettings::_intVal(const string& key) {
	if (!exists(intMap, key)) {
		intMap[key] = _intValFromJson(jsonStore, key);
	}

//    printMap(intMap, "ints");
	return intMap[key];
}

float& ofxJsonSettings::_floatVal(const string& key) {
	if (!exists(floatMap, key)) {
		floatMap[key] = _floatValFromJson(jsonStore, key);
	}

//    printMap(floatMap, "floats");
	return floatMap[key];
}

double& ofxJsonSettings::_doubleVal(const string& key) {
	if (!exists(doubleMap, key)) {
		doubleMap[key] = _doubleValFromJson(jsonStore, key);
	}

//    printMap(doubleMap, "doubles");
	return doubleMap[key];
}

ofVec2f& ofxJsonSettings::_vec2Val(const string& key) {
	if (!exists(vec2Map, key)) {
		vec2Map[key] = _vec2ValFromJson(jsonStore, key);
	}

//    printMap(vec2Map, "vec2s");
	return vec2Map[key];
}

ofVec3f& ofxJsonSettings::_vec3Val(const string& key) {
	if (!exists(vec3Map, key)) {
		vec3Map[key] = _vec3ValFromJson(jsonStore, key);
	}

//    printMap(vec3Map, "vec3s");
	return vec3Map[key];
}

ofVec4f& ofxJsonSettings::_vec4Val(const string& key) {
	if (!exists(vec4Map, key)) {
		vec4Map[key] = _vec4ValFromJson(jsonStore, key);
	}

//    printMap(vec4Map, "vec4s");
	return vec4Map[key];
}

ofColor& ofxJsonSettings::_colorVal(const string& key) {
	if (!exists(colorMap, key)) {
		colorMap[key] = _colorValFromJson(jsonStore, key);
	}

//    printMap(colorMap, "colors");
	return colorMap[key];
}

ofJson ofxJsonSettings::_jsonVal(const string& key) {
	if (ofStringTimesInString(key, delimiter)) {
		return getNestedChild(jsonStore, key);
	} else if (jsonStore.contains(key)) {
		return jsonStore[key];
	}
	return ofJson();
}

string ofxJsonSettings::_stringValFromJson(ofJson& data, const string & key) {
	try {
		// For keys like "fonts/face/serif", split the string and do a recursive
		// lookup to find that json element
		if (ofStringTimesInString(key, delimiter)) {
			return getNestedChild(data, key);
		}
		// Otherwise do a direct lookup if the key exists
		else if (data.contains(key)) {
			ofJson dd = getNestedChild(data, key);
			if(dd.is_string()) return dd;
			else return "";
		}
		else {
			ofLogWarning("Settings") << "no setting found for: " << key;
			return "";
		}
	} catch (const runtime_error& e) {
		ofLogError("Settings") << "error for key: " << key << ": " << e.what();
		return "";
	}
}

bool ofxJsonSettings::_boolValFromJson(ofJson& data, const string & key) {
	// See _stringValFromJson() for explanation
	try {
		if (ofStringTimesInString(key, delimiter)) {
			return getNestedChild(data, key);
		} else if (data.contains(key)) {
			ofJson dd = getNestedChild(data, key);
			if(dd.is_boolean()) return dd;
			else return false;
		} else {
			ofLogWarning("Settings") << "no setting found for: " << key;
			return false;
		}
	} catch (const runtime_error& e) {
		ofLogError("Settings") << "error for key: " << key << ": " << e.what();
		return false;
	}
}

int ofxJsonSettings::_intValFromJson(ofJson& data, const string & key) {
	// See _stringValFromJson() for explanation
	try {
		if (ofStringTimesInString(key, delimiter)) {
			return getNestedChild(data, key);
		} else if (data.contains(key)) {
			ofJson dd = getNestedChild(data, key);
			if(dd.is_number()) return dd;
			else return 0;
		} else {
			ofLogWarning("Settings") << "no setting found for: " << key;
			return 0;
		}
	} catch (const runtime_error& e) {
		ofLogError("Settings") << "error for key: " << key << ": " << e.what();
		return 0;
	}
}

float ofxJsonSettings::_floatValFromJson(ofJson& data, const string & key) {
	// See _stringValFromJson() for explanation
	try {
		if (ofStringTimesInString(key, delimiter)) {
			ofJson dd = getNestedChild(data, key);
			if(dd.is_number()) return dd;
			else return 0;
		} else if (data.contains(key)) {
			return data[key];
		} else {
			ofLogWarning("Settings") << "no setting found for: " << key;
			return 0;
		}
	} catch (const runtime_error& e) {
		ofLogError("Settings") << "error for key: " << key << ": " << e.what();
		return 0;
	}
}

double ofxJsonSettings::_doubleValFromJson(ofJson& data, const string & key) {
	// See _stringValFromJson() for explanation
	try {
		if (ofStringTimesInString(key, delimiter)) {
			return getNestedChild(data, key);
		} else if (data.contains(key)) {
			ofJson dd = getNestedChild(data, key);
			if(dd.is_number()) return dd;
			else return 0;
		} else {
			ofLogWarning("Settings") << "no setting found for: " << key;
			return 0;
		}
	} catch (const runtime_error& e) {
		ofLogError("Settings") << "error for key: " << key << ": " << e.what();
		return 0;
	}
}

ofVec2f ofxJsonSettings::_vec2ValFromJson(ofJson& data, const string & key) {
	// See _stringValFromJson() for explanation
	ofVec2f vec;

	try {
		if (ofStringTimesInString(key, delimiter)) {
			vec.x = getNestedChild(data, key)[0];
			vec.y = getNestedChild(data, key)[1];
		} else if (data.contains(key)) {
			vec.x = data[key][0];
			vec.y = data[key][1];
		} else {
			ofLogWarning("Settings") << "no setting found for: " << key;
		}
	} catch (const runtime_error& e) {
		ofLogError("Settings") << "error for key: " << key << ": " << e.what();
	}

	return vec;
}

ofVec3f ofxJsonSettings::_vec3ValFromJson(ofJson& data, const string & key) {
	// See _stringValFromJson() for explanation
	ofVec3f vec;

	try {
		if (ofStringTimesInString(key, delimiter)) {
			vec.x = getNestedChild(data, key)[0];
			vec.y = getNestedChild(data, key)[1];
			vec.z = getNestedChild(data, key)[2];
		} else if (data.contains(key)) {
			vec.x = data[key][0];
			vec.y = data[key][1];
			vec.z = data[key][2];
		} else {
			ofLogWarning("Settings") << "no setting found for: " << key;
		}
	} catch (const runtime_error& e) {
		ofLogError("Settings") << "error for key: " << key << ": " << e.what();
	}

	return vec;
}

ofVec4f ofxJsonSettings::_vec4ValFromJson(ofJson& data, const string & key) {
	// See _stringValFromJson() for explanation
	ofVec4f vec;

	try {
		if (ofStringTimesInString(key, delimiter)) {
			vec.x = getNestedChild(data, key)[0];
			vec.y = getNestedChild(data, key)[1];
			vec.z = getNestedChild(data, key)[2];
			vec.w = getNestedChild(data, key)[3];
		} else if (data.contains(key)) {
			vec.x = data[key][0];
			vec.y = data[key][1];
			vec.z = data[key][2];
			vec.w = data[key][3];
		} else {
			ofLogWarning("Settings") << "no setting found for: " << key;
		}
	} catch (const runtime_error& e) {
		ofLogError("Settings") << "error for key: " << key << ": " << e.what();
	}

	return vec;
}

ofColor ofxJsonSettings::_colorValFromJson(ofJson& data, const string & key) {
	// See _stringValFromJson() for explanation
	ofColor c;

	try {
		if (ofStringTimesInString(key, delimiter)) {
			ofJson j = getNestedChild(data, key);
			if(j.size() == 1){ //read grayscale color
				c.r = c.g = c.b = j[0];
			}else{ //read RGB[A] color
				c.r = j[0];
				c.g = j[1];
				c.b = j[2];
				if(j.size() > 3) c.a = j[3];
			}
		} else if (data.contains(key)) {
			if(data[key][0].size() == 1){ //read grayscale color
				c.r = c.g = c.b = data[key][0];
			}else{
				c.r = data[key][0];
				c.g = data[key][1];
				c.b = data[key][2];
				if(data[key].size() > 3) c.a = data[key][3];
			}
		} else {
			ofLogWarning("Settings") << "no setting found for: " << key;
		}
	} catch (const runtime_error& e) {
		ofLogError("Settings") << "error for key: " << key << ": " << e.what();
	}

	return c;
}

bool ofxJsonSettings::_exists(const string & key){
	try {
		if (ofStringTimesInString(key, delimiter))
			return (getNestedChild(jsonStore, key, true) != ofJson());
		else
			return jsonStore.contains(key);
	} catch (const runtime_error& e) {
		ofLogError("Settings") << "error for key: " << key << ": " << e.what();
		return false;
	}
}

bool ofxJsonSettings::_remove(const string & key){
	try {
		if (ofStringTimesInString(key, delimiter)) {
			// TODO: removing non top-level data
			ofLogWarning("Settings") << "remove() is only support on top-level (not nested) keys!";
			return false;
		}else {
			jsonStore.erase(key);
			stringMap.erase(key);
			intMap.erase(key);
			boolMap.erase(key);
			floatMap.erase(key);
			doubleMap.erase(key);
			vec2Map.erase(key);
			vec3Map.erase(key);
			vec4Map.erase(key);
			colorMap.erase(key);
			return true;
		}
	} catch (const runtime_error& e) {
		ofLogError("Settings") << "error for key: " << key << ": " << e.what();
		return false;
	}
}

ofJson ofxJsonSettings::getNestedChild(ofJson & data, const string & key, bool supressWarning) {
	auto keys = ofSplitString(key, delimiter);
	return getNestedChild(data, keys, supressWarning);
}

ofJson ofxJsonSettings::getNestedChild(ofJson & data, vector<string> & keys, bool supressWarning) {
	// Given a lookup key like "fonts/face/serif", find the corresponding
	// json object data["fonts"]["face"]["serif"]
	// (The other signature of this function actually splits the string into a
	// vector of keys)
	if (keys.size()) {
		string key = keys.front();
		keys.erase(keys.begin());

		if (data.contains(key)) {
			return getNestedChild(data[key], keys, supressWarning);
		} else {
			if (!supressWarning)
				ofLogWarning("Settings") << "no setting found for: " << key;

			return ofJson();
		}
	}

	return data;
}

template<typename T>
void ofxJsonSettings::setNestedChild(ofJson& data, const string & key, T val) {
	auto keys = ofSplitString(key, delimiter);
	setNestedChild(data, keys, val);
}

template<typename T>
void ofxJsonSettings::setNestedChild(ofJson& data, vector<string> & keys, T val) {
	// Given a lookup key like "fonts/face/serif", find the corresponding
	// json object data["fonts"]["face"]["serif"] and set its value
	// (The other signature of this function actually splits the string into a
	// vector of keys)
	auto* object = &data;

	if (keys.size() < 2) {
		ofLogError("Settings")
		<< "setNestedChild() malformed key (should be at least object/key): "
		<< ofJoinString(keys, delimiter);
		return;
	}

	while (keys.size()) {
		string key = keys.front();
		if (keys.size() == 1) {
			(*object)[key] = val;
			return;
		} else {
			keys.erase(keys.begin());
			object = (ofJson *) &(*object)[key];
		}
	}
}

template<typename T>
void ofxJsonSettings::cacheToJson(T& container, ofJson& data) {
	for (auto& it : container) {
		string key = it.first;
		if (ofStringTimesInString(key, delimiter)) {
			setNestedChild(data, key, it.second);
		} else {
			data[key] = container[key];
		}
	}
}

void ofxJsonSettings::cacheToJson(unordered_map<string,ofVec2f>& container, ofJson& data) {
	for (auto& it : container) {
		string key = it.first;

		ofJson array;
		array.push_back(it.second.x);
		array.push_back(it.second.y);

		if (ofStringTimesInString(key, delimiter)) {
			setNestedChild(data, key, array);
		} else {
			data[key] = array;
		}
	}
}

void ofxJsonSettings::cacheToJson(unordered_map<string,ofVec3f>& container, ofJson& data) {
	for (auto& it : container) {
		string key = it.first;

		ofJson array;
		array.push_back(it.second.x);
		array.push_back(it.second.y);
		array.push_back(it.second.z);

		if (ofStringTimesInString(key, delimiter)) {
			setNestedChild(data, key, array);
		} else {
			data[key] = array;
		}
	}
}

void ofxJsonSettings::cacheToJson(unordered_map<string,ofVec4f>& container, ofJson& data) {
	for (auto& it : container) {
		string key = it.first;

		ofJson array;
		array.push_back(it.second.x);
		array.push_back(it.second.y);
		array.push_back(it.second.z);
		array.push_back(it.second.w);

		if (ofStringTimesInString(key, delimiter)) {
			setNestedChild(data, key, array);
		} else {
			data[key] = array;
		}
	}
}

void ofxJsonSettings::cacheToJson(unordered_map<string,ofColor>& container, ofJson& data) {
	for (auto& it : container) {
		string key = it.first;

		ofJson array;
		array.push_back(it.second.r);
		array.push_back(it.second.g);
		array.push_back(it.second.b);

		if (ofStringTimesInString(key, delimiter)) {
			setNestedChild(data, key, array);
		} else {
			data[key] = array;
		}
	}
}

template<typename T>
bool ofxJsonSettings::exists(T& container, const string &key) {
	return container.find(key) != container.end();
}

template<typename T>
void ofxJsonSettings::printMap(T& container, const string& text) {
	for (auto& it : container) {
		ofLogNotice() << text << ": " << it.first << ": " << it.second;
	}
}
