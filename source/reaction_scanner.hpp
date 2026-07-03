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
#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include "matcher.hpp"
#include "query.hpp"
#include "reaction_parser.hpp"

namespace ckgrep {
/**
 * @brief Everything that shapes a search besides the query itself.
 *
 * @details Mirrors the CLI flags: @ref mode is `-e/--exact`, @ref
 * search_comments is `-c/--comments`, @ref pretty is `-p/--pretty`. With
 * @ref search_comments on, text behind a '!' is not discarded a priori: it
 * goes through the same reaction pipeline as live lines, so a commented-out
 * reaction that matches the query is a hit while prose comments never match.
 * With @ref pretty on, hits carry format_reaction() output instead of the
 * raw line -- normalized spacing, aligned rate columns, comments dropped
 * (including the '!' of a commented-out reaction).
 */
struct search_options {
  match_mode mode = match_mode::contains;  ///< How strictly reactions must match.
  bool search_comments = false;  ///< Also match commented-out reactions (the -c flag).
  bool pretty = false;  ///< Reformat hits via format_reaction() (the -p flag).
};

/**
 * @brief One matched line of a searched text.
 *
 * @details @ref text is the whole line as written in the source (trimmed),
 * inline comment included -- what you grep is what you can copy back into a
 * mechanism file.
 */
struct search_hit {
  std::size_t line_number = 0;  ///< 1-based line in the source text.
  std::string text;             ///< The matched line, trimmed, comment included.
};

/**
 * @brief Walks text one physical line at a time, invoking a callback per line.
 *
 * @details Splits @p text on '\n' and calls `fn(line, line_number)` once per
 * physical line (1-based), including a trailing line with no terminating
 * newline. Shared by scan_reactions() and search_text() so both see the
 * exact same line/line-number splitting.
 *
 * @tparam Fn        Callable taking (std::string_view line, std::size_t line_number).
 * @param text       The full text of a mechanism file.
 * @param fn         Invoked once per physical line.
 */
template <class Fn>
void for_each_line(std::string_view text, Fn&& fn) {
  std::size_t line_number = 0;
  std::size_t pos = 0;

  while (pos <= text.size()) {
    std::size_t nl = text.find('\n', pos);
    std::string_view line = text.substr(
        pos,
        nl == std::string_view::npos ? std::string_view::npos : nl - pos
    );
    ++line_number;

    fn(line, line_number);

    if (nl == std::string_view::npos) {
      break;
    }
    pos = nl + 1;
  }
}

/**
 * @brief Decides whether a physical line is a reaction line.
 *
 * @details A line counts as a reaction line if it is non-blank, is not a
 * comment (does not start with '!'), is not a known CHEMKIN keyword or
 * sub-data line (LOW, TROE, PLOG, ...), and contains a direction arrow
 * (`<=>`, `=>`, or `=`).
 *
 * @param line One physical line, not yet trimmed.
 * @return true if @p line looks like a reaction line.
 */
bool is_reaction_line(std::string_view line);

/**
 * @brief Scans a whole mechanism/text buffer and returns every reaction found.
 *
 * @details Comments and keyword blocks are skipped, not treated as errors.
 * Each candidate line is parsed with parse_reaction_line(); the parser does
 * not know where its text came from, so the 1-based line number is stamped
 * on each result here.
 *
 * @param text The full text of a mechanism file.
 * @return Every reaction found, in source order.
 */
std::vector<parsed_reaction> scan_reactions(std::string_view text);

/**
 * @brief Searches a whole mechanism/text buffer and returns every matching line.
 *
 * @details Walks the text once, line by line. Each line is split at its '!'
 * into a reaction part and a comment part; the reaction part is parsed and
 * tested against @p q (see matches()), and -- when
 * search_options::search_comments is set -- the comment part (truncated at
 * any further '!') is run through the exact same pipeline, so a commented-out
 * reaction is found while prose comments never match. A line that matches
 * both ways is still one hit: the single pass renders one verdict per line
 * before moving on, so results are deduplicated by construction and arrive
 * in line order.
 *
 * @param text The full text to search, e.g. one mechanism file's content.
 * @param q    The parsed query to test reactions against.
 * @param opts Match mode and comment-search settings; see search_options.
 * @return Every matching line, in source order, one entry per line.
 */
std::vector<search_hit>
search_text(std::string_view text, const query& q, const search_options& opts);

// Keyword / sub-data lines that are not reactions.
static constexpr std::array<std::string_view, 20> keywords = {
    "ELEM",      "ELEMENTS", "SPEC",      "SPECIES", "THER", "THERMO", "REAC",
    "REACTIONS", "END",      "PLOG",      "LOW",     "HIGH", "TROE",   "SRI",
    "REV",       "DUP",      "DUPLICATE", "FORD",    "RORD", "UNITS"
};

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
