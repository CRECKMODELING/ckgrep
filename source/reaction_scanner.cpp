/* ----------------------------------------------------------------------------------- *\
|                                 __                                                    |
|                           _____/ /______ _________  ____                              |
|                          / ___/ //_/ __ `/ ___/ _ \/ __ \                             |
|                         / /__/ ,< / /_/ / /  /  __/ /_/ /                             |
|                         \___/_/|_|\__, /_/   \___/ .___/                              |
|                                  /____/         /_/                                   |
|                                                                                       |
| ------------------------------------------------------------------------------------- |
|  See license and copyright at the end of this file.                                   |
| ------------------------------------------------------------------------------------- |
|                                                                                       |
|          Author: Timoteo Dinelli  <timoteo.dinelli@polimi.it>                         |
|                                                                                       |
|                  CRECK Modeling Lab <https://www.creckmodeling.polimi.it>             |
|                  Department of Chemistry, Materials and Chemical Engineering          |
|                  Politecnico di Milano, P.zza Leonardo da Vinci 32, 20133 Milano      |
|                                                                                       |
\* ----------------------------------------------------------------------------------- */
#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "reaction_parser.hpp"
#include "reaction_scanner.hpp"
#include "string_utils.hpp"

namespace ckgrep {

bool is_reaction_line(std::string_view line) {
  std::string_view t = utils::trim(line);
  if (t.empty()) {
    return false;
  }
  if (t.front() == '!') {  // comment
    return false;
  }
  // Keyword / sub-data lines that are not reactions.
  static constexpr std::array<std::string_view, 20> keywords = {
      "ELEM",      "ELEMENTS", "SPEC",      "SPECIES", "THER", "THERMO", "REAC",
      "REACTIONS", "END",      "PLOG",      "LOW",     "HIGH", "TROE",   "SRI",
      "REV",       "DUP",      "DUPLICATE", "FORD",    "RORD", "UNITS"
  };
  for (std::string_view kw : keywords) {
    if (utils::starts_with_ci(t, kw)) {
      return false;
    }
  }
  // Must contain a direction arrow / equals to be a reaction.
  return find_arrow(t).pos != std::string_view::npos;
}

std::vector<parsed_reaction> scan_reactions(std::string_view text) {
  std::vector<parsed_reaction> result;

  for_each_line(text, [&](std::string_view line, std::size_t line_number) {
    // Drop inline comments (everything after '!').
    if (std::size_t bang = line.find('!'); bang != std::string_view::npos) {
      line = line.substr(0, bang);
    }
    if (!is_reaction_line(line)) {
      return;
    }
    if (std::optional<parsed_reaction> r = parse_reaction_line(line)) {
      // The parser is a pure text -> structure function; where the line came
      // from is the scanner's knowledge, so the line number is stamped here.
      r->line_number = line_number;
      result.push_back(std::move(*r));
    }
  });

  return result;
}

std::vector<search_hit> search_text(
    std::string_view text,
    const query& q,
    const search_options& opts
) {
  std::vector<search_hit> hits;

  for_each_line(text, [&](std::string_view line, std::size_t line_number) {
    // One verdict per line: split at '!' into reaction and comment parts,
    // test each against its own kind of match, emit at most one hit.
    const std::size_t bang = line.find('!');
    const std::string_view reaction_part =
        (bang == std::string_view::npos) ? line : line.substr(0, bang);

    bool hit = false;
    if (is_reaction_line(reaction_part)) {
      if (std::optional<parsed_reaction> r = parse_reaction_line(reaction_part);
          r && matches(q, *r, opts.mode)) {
        hit = true;
      }
    }
    if (!hit && opts.search_comments && bang != std::string_view::npos) {
      std::string_view comment_part = utils::trim(line.substr(bang + 1));
      if (!comment_part.empty() &&
          utils::contains_ci(comment_part, opts.comment_needle)) {
        hit = true;
      }
    }
    if (hit) {
      hits.push_back({line_number, std::string(utils::trim(line))});
    }
  });

  return hits;
}

}  // namespace ckgrep
/* ----------------------------------------------------------------------------------- *\
|                                                                                       |
|     GNU General Public License v3 (GPL-3)                                             |
|                                                                                       |
|     Copyright (c) 2026 Timoteo Dinelli                                                |
|                                                                                       |
|     This program is free software: you can redistribute it and/or modify it           |
|     under the terms of the GNU General Public License as published by the Free        |
|     Software Foundation, either version 3 of the License, or (at your option)         |
|     any later version.                                                                |
|                                                                                       |
|     This program is distributed in the hope that it will be useful, but WITHOUT       |
|     ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS     |
|     FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.    |
|                                                                                       |
|     You should have received a copy of the GNU General Public License along with      |
|     this program. If not, see <https://www.gnu.org/licenses/>.                        |
|                                                                                       |
\* ----------------------------------------------------------------------------------- */
