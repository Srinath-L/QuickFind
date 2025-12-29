#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <thread>
#include <mutex>
#include <chrono>
#include <cctype>

using namespace std;
namespace fs = std::filesystem;

unordered_map<string, vector<string>> index_map;
mutex index_mutex;

vector<string> ignore_dirs = {
    "Windows",
    "Program Files",
    "Program Files (x86)",
    ".git",
    "node_modules"};

string to_lower(string s)
{
    for (char &c : s)
        c = tolower(c);
    return s;
}

bool should_ignore(const fs::path &p)
{
    string path_str = p.string();
    for (const auto &dir : ignore_dirs)
    {
        if (path_str.find(dir) != string::npos)
            return true;
    }
    return false;
}

struct TrieNode
{
    unordered_map<char, TrieNode *> children;
    bool is_end = false;
};

TrieNode *trie_root = new TrieNode();

void trie_insert(const string &word)
{
    TrieNode *node = trie_root;
    for (char c : word)
    {
        if (!node->children[c])
            node->children[c] = new TrieNode();
        node = node->children[c];
    }
    node->is_end = true;
}

void trie_collect(TrieNode *node, string current, vector<string> &result)
{
    if (!node)
        return;

    if (node->is_end)
        result.push_back(current);

    for (auto &p : node->children)
        trie_collect(p.second, current + p.first, result);
}

vector<string> trie_prefix_search(const string &prefix)
{
    TrieNode *node = trie_root;
    for (char c : prefix)
    {
        if (!node->children.count(c))
            return {};
        node = node->children[c];
    }

    vector<string> result;
    trie_collect(node, prefix, result);
    return result;
}

void index_files(const string &path)
{
    try
    {
        for (const auto &entry : fs::directory_iterator(path))
        {
            if (entry.is_directory())
            {
                if (!should_ignore(entry.path()))
                    index_files(entry.path().string());
            }
            else if (entry.is_regular_file())
            {
                string full_path = entry.path().string();
                string filename = to_lower(entry.path().filename().string());

                lock_guard<mutex> lock(index_mutex);
                index_map[filename].push_back(full_path);
                trie_insert(filename);
            }
        }
    }
    catch (...)
    {
    }
}

void search_files(const string &query)
{
    unordered_set<string> printed;
    int total = 0;

    cout << "\nExact matches:\n";
    auto exact = index_map.find(query);
    if (exact != index_map.end())
    {
        for (const auto &path : exact->second)
        {
            cout << "  " << path << "\n";
            total++;
        }
        printed.insert(query);
    }
    else
    {
        cout << "  (none)\n";
    }

    cout << "\nPrefix matches:\n";
    vector<string> prefix_matches = trie_prefix_search(query);
    bool has_prefix = false;

    for (const auto &fname : prefix_matches)
    {
        if (printed.count(fname))
            continue;

        for (const auto &path : index_map[fname])
        {
            cout << "  " << path << "\n";
            total++;
            has_prefix = true;
        }
        printed.insert(fname);
    }

    if (!has_prefix)
        cout << "  (none)\n";

    cout << "\nSubstring matches:\n";
    bool has_sub = false;

    for (const auto &pair : index_map)
    {
        const string &fname = pair.first;

        if (printed.count(fname))
            continue;

        if (fname.find(query) != string::npos)
        {
            for (const auto &path : pair.second)
            {
                cout << "  " << path << "\n";
                total++;
                has_sub = true;
            }
            printed.insert(fname);
        }
    }

    if (!has_sub)
        cout << "  (none)\n";

    cout << "\n----------------------\n";
    cout << "Total results: " << total << "\n";
}

int main()
{
    string root_path;
    cout << "Enter directory to index: ";
    getline(cin, root_path);

    if (!fs::exists(root_path))
    {
        cout << "Invalid directory path\n";
        return 0;
    }

    cout << "Indexing files...\n";
    auto start = chrono::high_resolution_clock::now();

    vector<thread> threads;

    try
    {
        for (const auto &entry : fs::directory_iterator(root_path))
        {
            if (entry.is_directory())
            {
                if (!should_ignore(entry.path()))
                    threads.emplace_back(index_files, entry.path().string());
            }
            else if (entry.is_regular_file())
            {
                string full_path = entry.path().string();
                string filename = to_lower(entry.path().filename().string());

                lock_guard<mutex> lock(index_mutex);
                index_map[filename].push_back(full_path);
                trie_insert(filename);
            }
        }
    }
    catch (...)
    {
        
    }

    for (auto &t : threads)
        t.join();

    auto end = chrono::high_resolution_clock::now();

    cout << "Indexing completed in "
         << chrono::duration_cast<chrono::milliseconds>(end - start).count()
         << " ms\n";

    cout << "Indexed " << index_map.size() << " unique filenames\n";

    while (true)
    {
        string query;
        cout << "\nSearch (type 'exit' to quit): ";
        getline(cin, query);

        if (query == "exit")
            break;

        query = to_lower(query);
        search_files(query);
    }

    return 0;
}