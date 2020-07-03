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
/**
 * @file
 * @brief Header of the semver::version class.
 *
 * This header defines the semver::version class, the central type of the C++
 * API of `libsemver` library.
 *
 * @copyright Copyright (c) 2016 Enrico M. Crisostomo
 * @license GNU General Public License v. 3.0
 * @author Enrico M. Crisostomo
 * @version 1.0.0
 */
#ifndef SEMVER_UTILS_VERSION_H
#define SEMVER_UTILS_VERSION_H

#include <vector>
#include <string>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

/**
 * @brief Main namespace of `libsemver`.
 */
namespace semver
{
  /**
   * @brief Class that represents a prerelease identifier.
   *
   * This class represents a prerelease identifier complying with _Semantic
   * Versioning_ 2.0.0 (http://semver.org/).
   */
  class prerelease_component
  {
  private:
    /**
     * @brief Indicates whether the component is a number.
     */
    bool is_number;

    /**
     * @brief The component representation as a string.
     */
    std::string identifier;

    /**
     * @brief The component numeric value.
     *
     * This field is meaningful only if the current component is a number.
     */
    unsigned long value_as_number;

  public:
    prerelease_component(std::string s);
    bool operator<(const prerelease_component& rh) const;
    bool operator>(const prerelease_component& rh) const;
    bool operator==(const prerelease_component& v) const;
  };

  /**
   * @brief Class that represents a version number.
   *
   * This class represents a version number complying with _Semantic Versioning_
   * 2.0.0 (http://semver.org/).  As a supported extension this class allows
   * version numbers to contain any number of components greater than 2.
   *
   * Instances of this class are designed to be immutable.
   */
  class version
  {
  private:
    std::vector<unsigned int> versions;
    std::string prerelease;
    std::vector<prerelease_component> prerelease_comp;
    std::string metadata;

    void parse_prerelease();

  public:
    /**
     * @brief Constructs a semver::version instance from a std::string.
     *
     * The version number @p v must comply with _Semantic Versioning 2.0.0_.  As
     * a supported extension this class allows version numbers to contain any
     * number of components greater than 2.
     *
     * @param v The version number to parse.
     * @return A semver::version instance.
     * @throws std::invalid_argument if @p v is not a valid version number.
     */
    static version from_string(std::string v);

    /**
     * @brief Constructs a semver::version instance with the specified
     * parameters.  The parameters must comply with _Semantic Versioning 2.0.0_.
     *
     * @param versions The version components.  As a supported extension this
     * class allows versions numbers to contain any number of components greater
     * than 2.
     * @param prerelease An optional prerelease string.
     * @param metadata An optional metadata string.
     * @return A semver::version instance.
     * @throws std::invalid_argument if the parameters do not comply with
     * _Semantic Versioning 2.0.0_.
     */
    version(std::vector<unsigned int> versions,
            std::string prerelease = "",
            std::string metadata = "");

    /**
     * @brief Converts a version to its string representation.
     *
     * @return The string representation of this instance.
     */
    std::string str() const;

    /**
     * @brief Gets the version components.
     *
     * @return The version components.
     */
    std::vector<unsigned int> get_version() const;

    /**
     * @brief Gets the specified version component.
     *
     * @return The specified version component, or 0 if the specified @p index
     * does not exist.
     */
    unsigned int get_version(unsigned int index) const;

    /**
     * @brief Gets the prerelease string.
     *
     * @return The prerelease string.
     */
    std::string get_prerelease() const;

    /**
     * @brief Gets the metadata string.
     *
     * @return The metadata string.
     */
    std::string get_metadata() const;

    /**
     * @brief Bumps the major version component.
     *
     * This method uses ::bump();
     *
     * @return A semver::version object containing the bumped version.
     */
    version bump_major() const;

    /**
     * @brief Bumps the minor version component.
     *
     * This method uses ::bump();
     *
     * @return A semver::version object representing the bumped version.
     */
    version bump_minor() const;

    /**
     * @brief Bumps the patch version component.
     *
     * This method uses ::bump();
     *
     * @return A semver::version object representing the bumped version.
     */
    version bump_patch() const;

    /**
     * @brief Bumps the specified version component.
     *
     * The component @p index is bumped and all components whose index is grater
     * than @p index are set to 0.  If @p index is greater than the current size
     * `s` of the version number, then `index + 1 - s` components are added and
     * set to 0.
     *
     * @return A semver::version object representing the bumped version.
     */
    version bump(unsigned int index) const;

    /**
     * @brief Strips the prerelease component.
     *
     * @return A semver::version object representing the modified version.
     */
    version strip_prerelease() const;

    /**
     * @brief Strips the metadata component.
     *
     * @return A semver::version object representing the modified version.
     */
    version strip_metadata() const;

    /**
     * @brief Checks whether the instance is a release version.
     *
     * @return `true` if the instance represents a release version, `false`
     * otherwise.
     */
    bool is_release() const;

    /**
     * @brief Checks two instances for equality.
     *
     * Two instances are equal if and only if all its components are equal.
     *
     * @param v The instance to be compared with.
     * @return `true` if @p rh is equal to this instance, `false` otherwise.
     */
    bool operator==(const version& rh) const;

    /**
     * @brief Compares two instances.
     *
     * An instance is less than another if its version number is less than the
     * other's.  Components are compared one by one, starting from the first.
     * If the two components are equal, then the following component is
     * considered.  If the first component is less than the second, then `true`
     * is returned, otherwise `false` is returned.
     *
     * @param v The instance to be compared with.
     * @return `true` if this instance is less than @p rh, `false` otherwise.
     */
    bool operator<(const version& rh) const;

    /**
     * @brief Compares two instances.
     *
     * An instance is greater than another if its version number is less than
     * the other's.  Components are compared one by one, starting from the
     * first.  If the two components are equal, then the following component is
     * considered.  If the first component is greater than the second, then
     * `true` is returned, otherwise `false` is returned.
     *
     * @param v The instance to be compared with.
     * @return `true` if this instance is greater than @p rh, `false` otherwise.
     */
    bool operator>(const version& rh) const;
  };
}

#endif // SEMVER_UTILS_VERSION_H

#pragma clang diagnostic pop