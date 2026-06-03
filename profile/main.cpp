#include <benchmark/benchmark.h>
#include <typeahead/search.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>

void resolve_directories() // Ensures the res/ directory can be found from the current working directory
{
    #ifdef PREFERRED_WORKING_DIRECTORY
    std::filesystem::current_path(PREFERRED_WORKING_DIRECTORY); // Updates the current working directory
    #endif
    if ( !std::filesystem::exists(std::filesystem::path("res/")) )
        throw std::runtime_error("Could not locate the \"res/\" directory in the current working directory");
}

std::string load_raw_book()
{
    resolve_directories();
    //std::ifstream book_file("res/combined_europarl.txt");
    std::ifstream book_file("res/dracula.txt");
    if ( !book_file )
        throw std::runtime_error("Error locating book file!");

    std::vector<char> buffer{std::istreambuf_iterator<char>(book_file), std::istreambuf_iterator<char>()};
    if ( !book_file )
        throw std::runtime_error("Error reading book file!");

    return std::string(buffer.begin(), buffer.end());
}

std::string_view view_book()
{
    static std::string raw_book = load_raw_book();
    return std::string_view(raw_book);
}

std::vector<std::string> split_book(std::string_view book = view_book())
{
    std::vector<std::string> split_contents {};
    std::size_t size = book.size();
    std::size_t pos = 0;
    while ( pos < size )
    {
        auto next_break = book.find_first_of(".", pos);
        for ( std::size_t i=0; i<458; ++i )
        {
            auto next_br = book.find_first_of(".", next_break);
            if ( next_br == std::string_view::npos )
            {
                next_break = book.size();
                break;
            }

            // Skip to the end of ellipses and post-period close-quotes
            while ( next_br < size-2 && (book[next_br+1] == '.' || book[next_br+1] == '"' || book[next_br+1] == '\'') )
                ++next_br;

            next_break = next_br;
        }

        if ( next_break > pos )
        {
            split_contents.push_back(std::string(book.substr(pos, next_break-pos+1)));
            pos = next_break+1;
        }
        else
        {
            ++pos;
            while ( pos < size-1 && std::isspace(static_cast<unsigned char>(book[pos])) )
                ++pos;
        }
    }
    return split_contents;
}

static void bm_build_search_cache(benchmark::State & state)
{
    auto book = split_book();
    for ( auto _ : state ) {
        search::strings search_book {};
        search_book.load(book);
    }
}

static void bm_add_search_item(benchmark::State & state)
{
    auto book = split_book();
    search::strings search_book {};
    search_book.load(book);
    int i = 0;
    for ( auto _ : state ) {
        search_book.item_added<true>(0, "new search item" + std::to_string(i++));
    }
}

static void bm_remove_search_item(benchmark::State & state)
{
    auto book = split_book();
    search::strings search_book {};
    search_book.load(book);
    for ( auto _ : state ) {
        search_book.item_removed<true>(0);
    }
}

static void bm_modify_search_item(benchmark::State & state)
{
    auto book = split_book();
    search::strings search_book {};
    search_book.load(book);
    int i = 0;
    for ( auto _ : state ) {
        search_book.item_text_changed(0, std::to_string(i++));
    }
}

static void bm_perform_search(benchmark::State & state)
{
    auto book = split_book();
    search::strings search_book {};
    search_book.load(book);
    for ( auto _ : state ) {
        //benchmark::DoNotOptimize(search_book.search_for({.search_text = "Laperrouze"}));
        benchmark::DoNotOptimize(search_book.search_for({.search_text = "drac"}));
    }
}

BENCHMARK(bm_build_search_cache)->Unit(benchmark::kMillisecond);
BENCHMARK(bm_add_search_item)->Unit(benchmark::kMillisecond)->Iterations(1000);
BENCHMARK(bm_remove_search_item)->Unit(benchmark::kMillisecond)->Iterations(1000);
BENCHMARK(bm_modify_search_item)->Unit(benchmark::kMillisecond);
BENCHMARK(bm_perform_search)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();

/*#include <iostream>
int main()
{
    auto book = split_book();
    std::cout << book.size();
}*/
