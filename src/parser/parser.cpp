#include "parser.hpp"
#include "exhaustive_parser.hpp"
#include <fstream>
#include <print>

namespace parser {

Parser::Parser(std::filesystem::path path): path{std::move(path)} {}

auto Parser::parse() const -> ParsedScript {
    std::ifstream file{path};
    if (!file) {
        throw std::runtime_error("Cannot open file: " + path.string());
    }

    auto content{std::make_unique<const std::string>(
        std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()
    )};

    auto split_lines{
      [](std::string_view content) -> std::vector<std::string_view> {
          std::vector<std::string_view> lines;
          size_t start{0};
          while (start < content.size()) {
              const auto end{content.find('\n', start)};
              if (end == std::string_view::npos) {
                  lines.emplace_back(content.substr(start));
                  break;
              }
              lines.emplace_back(content.substr(start, end - start));
              start = end + 1;
          }
          return lines;
      }
    };

    const std::vector<std::string_view> lines{split_lines(*content)};
    std::vector<Instruction::Value> result;

    auto trim{[](std::string_view sv) -> std::string_view {
        const auto start{sv.find_first_not_of(" \t\r")};

        if (start == std::string_view::npos) {
            return {};
        }

        const auto end{sv.find_last_not_of(" \t\r")};
        return sv.substr(start, end - start + 1);
    }};

    for (const auto& l : lines) {
        auto line{trim(l)};

        if (line.empty()) {
            continue;
        }

        ParseResult<Instruction::Value> line_result{
          ExhaustiveParser<Instruction::Value>::run(line)
        };

        if (line_result) {
            result.emplace_back(std::move(*line_result));
        } else {
            // TODO: Show error on screen
            std::println("{}", line_result.error().message());
        }
    }

    return ParsedScript{std::move(content), std::move(result)};
}

} // namespace parser
