/*
 * Copyright (c) 2016-2017 Enrico M. Crisostomo
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <algorithm>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <vector>

namespace semver
{
    static std::vector<unsigned int> parse_version(const std::string &v)
    {
        std::regex numbers("\\d+");
        std::sregex_token_iterator first(v.begin(), v.end(), numbers);
        std::sregex_token_iterator last;

        std::vector<unsigned int> results;
        std::for_each(first, last, [&results](std::string s) { results.push_back((unsigned int) std::stoul(s)); });

        return results;
    }

    class version
    {
      private:
        class prerelease_component
        {
          private:
            bool is_number;
            std::string identifier;
            unsigned long value_as_number;

          public:
            prerelease_component(std::string s) : identifier(std::move(s))
            {
                is_number = !identifier.empty() && std::find_if(identifier.begin(), identifier.end(), [](char c) { return !std::isdigit(c); }) == identifier.end();
                value_as_number = is_number ? std::stoul(identifier) : 0;
            }

            bool operator==(const prerelease_component &v) const
            {
                return identifier == v.identifier;
            }

            bool operator<(const prerelease_component &rh) const
            {
                if (is_number && rh.is_number)
                    return value_as_number < rh.value_as_number;
                if (!is_number && !rh.is_number)
                    return identifier < rh.identifier;
                return is_number;
            }

            bool operator>(const prerelease_component &rh) const
            {
                return (rh < *this);
            }
        };

        const static inline std::string PRERELEASE_PATTERN{ "([0-9A-Za-z-]+(\\.[0-9A-Za-z-]+)*)" };
        const static inline std::string METADATA_PATTERN{ "([0-9A-Za-z-]+(\\.[0-9A-Za-z-]+)*)" };

        std::vector<unsigned int> versions;
        std::string prerelease;
        std::vector<prerelease_component> prerelease_comp;
        std::string metadata;

        void parse_prerelease()
        {
            std::regex separator("\\.");
            std::sregex_token_iterator first(prerelease.begin(), prerelease.end(), separator, -1);
            std::sregex_token_iterator last;

            std::for_each(first, last,
                          [this](std::string s)
                          {
                              [s]()
                              {
                                  if (s.empty())
                                      throw std::invalid_argument("Invalid identifier: " + s);

                                  if (s[0] != '0')
                                      return;

                                  for (size_t i = 1; i < s.size(); ++i)
                                  {
                                      if (!std::isdigit(s[i]))
                                          return;
                                  }

                                  throw std::invalid_argument("Numerical identifier cannot contain leading zeroes.");
                              }();
                              prerelease_comp.emplace_back(s);
                          });
        }

      public:
        static version from_string(std::string v)
        {
            const static std::string version_pattern = "((0|[1-9][[:digit:]]*)(\\.(0|[1-9][[:digit:]]*)){1,})";
            const static int VERSION_INDEX = 1;

            const static std::string prerelease_pattern = "(-" + PRERELEASE_PATTERN + ")?";
            const static int PRERELEASE_INDEX = 6;

            const static std::string metadata_pattern = "(\\+" + METADATA_PATTERN + ")?";
            const static int METADATA_INDEX = 9;

            const static std::string grammar = std::string("^") + version_pattern + prerelease_pattern + metadata_pattern + "$";
            const static std::regex version_grammar(grammar, std::regex_constants::extended);

            std::smatch fragments;
            if (!std::regex_match(v, fragments, version_grammar))
                throw std::invalid_argument("Invalid version: " + v);

            return semver::version(parse_version(fragments[VERSION_INDEX].str()), fragments[PRERELEASE_INDEX].str(), fragments[METADATA_INDEX].str());
        }

        version(std::vector<unsigned int> versions, std::string prerelease = "", std::string metadata = "")
            : versions(std::move(versions)), prerelease(std::move(prerelease)), metadata(std::move(metadata))
        {
            if (this->versions.size() < 2)
                throw std::invalid_argument("Version must contain at least two numbers.");

            if (!this->prerelease.empty())
            {
                if (!std::regex_match(this->prerelease, std::regex(PRERELEASE_PATTERN)))
                    throw std::invalid_argument("Invalid prerelease: " + this->prerelease);

                parse_prerelease();
            }

            if (!this->metadata.empty())
            {
                if (!std::regex_match(this->metadata, std::regex(METADATA_PATTERN)))
                    throw std::invalid_argument("Invalid metadata: " + this->metadata);
            }
        }

        std::string to_string() const
        {
            std::string out = std::to_string(versions[0]);

            for (size_t i = 1; i < versions.size(); ++i)
                out += "." + std::to_string(versions[i]);

            if (!prerelease.empty())
                out += "-" + prerelease;

            if (!metadata.empty())
                out += "+" + metadata;

            return out;
        }

        std::vector<unsigned int> get_version() const
        {
            return versions;
        }

        std::string get_prerelease() const
        {
            return prerelease;
        }

        std::string get_metadata() const
        {
            return metadata;
        }

        version strip_prerelease() const
        {
            return version(versions, "", metadata);
        }

        version strip_metadata() const
        {
            return version(versions, prerelease, "");
        }

        bool is_release() const
        {
            return (prerelease.empty());
        }

        bool operator==(const version &v) const
        {
            return versions == v.versions && prerelease == v.prerelease;
        }

        bool operator<(const version &v) const
        {
            // Compare version numbers.
            if (versions < v.versions)
                return true;
            if (versions > v.versions)
                return false;

            // Compare prerelease identifiers.
            if (prerelease == v.prerelease)
                return false;

            // If either one, but not both, are release versions, release is greater.
            if (is_release() ^ v.is_release())
                return !is_release();

            return prerelease_comp < v.prerelease_comp;
        }

        bool operator>(const version &v) const
        {
            return (v < *this);
        }
    };
} // namespace semver
