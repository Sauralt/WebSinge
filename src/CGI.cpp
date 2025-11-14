#include "../include/CGI.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>

std::string Contact::urlDecode(const std::string &str) {
    std::string result;
    char ch;
    int val;

    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '+') result.push_back(' ');
        else if (str[i] == '%' && i + 2 < str.size() &&
                 sscanf(str.substr(i+1, 2).c_str(), "%x", &val) == 1) {
            result.push_back(static_cast<char>(val));
            i += 2;
        } else result.push_back(str[i]);
    }
    return result;
}

std::map<std::string,std::string> Contact::parse(const std::string &query) {
    std::map<std::string,std::string> out;
    size_t pos = 0;

    while (pos < query.size()) {
        size_t eq = query.find('=', pos);
        size_t amp = query.find('&', pos);
        if (eq == std::string::npos) break;
        if (amp == std::string::npos) amp = query.size();

        std::string key = query.substr(pos, eq-pos);
        std::string value = query.substr(eq+1, amp - eq - 1);

        out[key] = urlDecode(value);

        pos = amp + 1;
    }
    return out;
}

void Contact::saveToFile(const std::map<std::string,std::string> &data,
                         const std::string &path) {
    std::ofstream file(path, std::ios::app);
    if (!file) return;

    file << "----- NEW ENTRY -----\n";
    for (auto &p : data)
        file << p.first << ": " << p.second << "\n";
    file << "\n";
}

std::string Contact::generateHtml(const std::map<std::string,std::string> &data) {
    std::stringstream html;

    html << "<html><body><h1>Données reçues</h1>";

    for (auto &p : data)
        html << "<p><b>" << p.first << "</b> : " << p.second << "</p>";

    html << "<a href=\"../contact.html\">Retour</a>";
    html << "</body></html>";

    return html.str();
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;

    Contact c;

    std::string query = argv[1];
    auto data = c.parse(query);
    c.saveToFile(data, "contact_data.txt");

    std::cout << c.generateHtml(data);
    return 0;
}
