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
#include <gtest/gtest.h>

#include "query.hpp"
#include "reaction_scanner.hpp"

namespace ckgrep {
TEST(IsReactionLine, AcceptsReactionRejectsKeywordAndCommentLines) {
  EXPECT_TRUE(is_reaction_line("H+O2=OH+O  1.0 0.0 0.0"));
  EXPECT_FALSE(is_reaction_line(""));
  EXPECT_FALSE(is_reaction_line("! comment"));
  EXPECT_FALSE(is_reaction_line("REACTIONS"));
  EXPECT_FALSE(is_reaction_line("TROE /0.7 100.0 1000.0/"));
  EXPECT_FALSE(is_reaction_line("low /1.0 2.0 3.0/"));  // keywords match any case
}

TEST(ScanReactions, StampsOneBasedLineNumbers) {
  const std::string_view text =
      "! header comment\n"
      "H2+O2<=>H2O+O  1.0e13 0.0 5000.0\n"
      "LOW /1.0 2.0 3.0/\n"
      "CH4+O2=>CO2+H2O  1.0e12 0.0 0.0\n";
  std::vector<parsed_reaction> reactions = scan_reactions(text);
  ASSERT_EQ(reactions.size(), 2U);
  EXPECT_EQ(reactions[0].line_number, 2U);
  EXPECT_EQ(reactions[1].line_number, 4U);
}

TEST(ScanReactions, InlineCommentStrippedFromRaw) {
  std::vector<parsed_reaction> reactions =
      scan_reactions("H+OH=H2O 1.0 0.0 0.0 ! note\n");
  ASSERT_EQ(reactions.size(), 1U);
  EXPECT_EQ(reactions[0].raw, "H+OH=H2O 1.0 0.0 0.0");
}

TEST(ScanReactions, FalloffClassificationSurvivesScanning) {
  std::vector<parsed_reaction> reactions =
      scan_reactions("H+O2(+M)=HO2(+M)   5.0e12  0.0  0.0\n");
  ASSERT_EQ(reactions.size(), 1U);
  EXPECT_EQ(reactions[0].third_body, third_body_kind::falloff);
  EXPECT_EQ(reactions[0].collider, "M");
}

// A small mechanism exercising every way a line can match: as a reaction, as
// a comment, or both at once (line 2).
constexpr std::string_view search_fixture =
    "! CH4 oxidation block\n"
    "H+OH=H2O 1.0 0.0 0.0 ! H2O formation note\n"
    "CH4+O2=CO2+H2O 1.0 0.0 0.0\n";

TEST(SearchText, ReactionHitsArriveInLineOrder) {
  query q = parse_query("H2O");
  std::vector<search_hit> hits = search_text(search_fixture, q, {});
  ASSERT_EQ(hits.size(), 2U);
  EXPECT_EQ(hits[0].line_number, 2U);
  EXPECT_EQ(hits[1].line_number, 3U);
}

TEST(SearchText, LineMatchingAsReactionAndCommentIsOneHit) {
  // Line 2 matches the query as a reaction AND carries "H2O" in its comment;
  // it must be reported exactly once.
  query q = parse_query("H2O");
  search_options opts;
  opts.search_comments = true;
  opts.comment_needle = "H2O";
  std::vector<search_hit> hits = search_text(search_fixture, q, opts);
  ASSERT_EQ(hits.size(), 2U);
  EXPECT_EQ(hits[0].line_number, 2U);
  EXPECT_EQ(hits[1].line_number, 3U);
}

TEST(SearchText, CommentOnlyLineFoundWithFlagInLineOrder) {
  query q = parse_query("H2O");
  search_options opts;
  opts.search_comments = true;
  opts.comment_needle = "oxidation";
  std::vector<search_hit> hits = search_text(search_fixture, q, opts);
  ASSERT_EQ(hits.size(), 3U);
  EXPECT_EQ(hits[0].line_number, 1U);  // full-line comment, before the reactions
  EXPECT_EQ(hits[1].line_number, 2U);
  EXPECT_EQ(hits[2].line_number, 3U);
}

TEST(SearchText, CommentMatchIsCaseInsensitive) {
  query q = parse_query("ZZZ");  // matches no reaction
  search_options opts;
  opts.search_comments = true;
  opts.comment_needle = "OXIDATION";
  std::vector<search_hit> hits = search_text(search_fixture, q, opts);
  ASSERT_EQ(hits.size(), 1U);
  EXPECT_EQ(hits[0].line_number, 1U);
}

TEST(SearchText, CommentsIgnoredWithoutFlag) {
  query q = parse_query("ZZZ");
  search_options opts;
  opts.comment_needle = "oxidation";  // set but search_comments stays false
  EXPECT_TRUE(search_text(search_fixture, q, opts).empty());
}

TEST(SearchText, HitTextIsFullLineWithComment) {
  query q = parse_query("H2O");
  std::vector<search_hit> hits = search_text(search_fixture, q, {});
  ASSERT_EQ(hits.size(), 2U);
  EXPECT_EQ(hits[0].text, "H+OH=H2O 1.0 0.0 0.0 ! H2O formation note");
}

TEST(SearchText, ExactModeForwardedToMatcher) {
  query q = parse_query("CH4=");
  search_options contains_opts;
  EXPECT_EQ(search_text(search_fixture, q, contains_opts).size(), 1U);
  search_options exact_opts;
  exact_opts.mode = match_mode::exact;
  // "CH4+O2=..." has a leftover O2 reactant, so exact mode must reject it.
  EXPECT_TRUE(search_text(search_fixture, q, exact_opts).empty());
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
