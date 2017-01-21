/*
 * Copyright 2017 Lorenz Haas <lorenz.haas@histomatics.de>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cppcrate/query.h>

namespace CppCrate {

/*!
 * \class CppCrate::Query
 *
 * \brief Provides the possibility to define SQL statements covering also parameter substitution and
 *        bulk operations.
 *
 * The class %Query provides the possibility to conveniently define simple SQL statements,
 * statements with parameter substitution, as well as statements for bulk operations. The actual
 * type of a %Query is accessible through type().
 *
 * All three types can be defined either by using the corresponding constructor or by using the
 * property based approach. To select all rows of a table called "players" write directly
 *
 * \code
 * Query q("SELECT * FROM players");
 * \endcode
 *
 * or set the appropriate properties individually
 *
 * \code
 * Query q;
 * q.setStatement("SELECT * FROM players");
 * \endcode
 *
 * In order to prepare a statement with arguments (parameter substitution) use placeholders:
 * \code
 * Query q("SELECT * FROM players WHERE age > ?", "[42]");
 * // or use numbered placeholders
 * Query q("SELECT * FROM players WHERE age > $1", "[42]");
 * \endcode
 *
 * For a bulk operation, finally, use:
 *
 * \code
 * Query q("INSERT INTO players (id, name) VALUES (?,?)");
 * std::vector<std::string> newPlayers;
 * newPlayers.push_back("[1, ’Calvin’]");
 * newPlayers.push_back("[2, ’Hobbes’]");
 * q.bulkArguments(newPlayers);
 * \endcode
 */

/*!
 * \enum Query::Type
 * Describes the query's type.
 *
 * \var Query::Type Query::SimpleType
 * A simple SQL statement.
 *
 * \var Query::Type Query::ArgumentType
 * A SQL statement with arguments.
 *
 * \var Query::Type Query::BulkArgumentType
 * A bulk operation.
 */

/// \cond INTERNAL
class Query::Private {
 public:
  bool operator==(const Private &other) const {
    return sql == other.sql && args == other.args && bulkArgs == other.bulkArgs;
  }
  std::string sql;
  std::string args;
  std::vector<std::string> bulkArgs;
};
/// \endcond

CPPCRATE_PIMPL_IMPLEMENT_ALL(Query)

/*!
 * Constructs a query with the SQL statement \a sql. If \a sql is empty the query is also empty.
 */
Query::Query(const std::string &sql) : p(new Private) { p->sql = sql; }

/*!
 * Constructs a query with the SQL statement \a sql and the parameters \a args.
 *
 * \pre \a args must be a well-formed JSON array and should not contain a null character.
 */
Query::Query(const std::string &sql, const std::string &args) : p(new Private) {
  p->sql = sql;
  p->args = args;
}

/*!
 * Constructs a query with the SQL statement \a sql and the bulk arguments \a bulkArgs.
 *
 * \pre The elements of \a bulkArgs must be well-formed JSON arrays and should not contain a null
 *      character.
 */
Query::Query(const std::string &sql, const std::vector<std::string> &bulkArgs) : p(new Private) {
  p->sql = sql;
  p->bulkArgs = bulkArgs;
}

/*!
 * Returns \c true if neither statement, nor arguments, nor bulk arguments are defined.
 */
bool Query::isEmpty() const { return p->sql.empty() && p->args.empty() && p->bulkArgs.empty(); }

/*!
 * Returns the query's type. An empty query is considered a simple type (SimpleType).
 */
Query::Type Query::type() const {
  return hasArguments() ? ArgumentType : (hasBulkArguments() ? BulkArgumentType : SimpleType);
}

/*!
 * Sets the SQL statement to \a sql.
 */
void Query::setStatement(const std::string &sql) { p->sql = sql; }

/*!
 * Returns the SQL statement.
 */
const std::string &Query::statement() const { return p->sql; }

/*!
 * Returns whether the query has a statement defined.
 */
bool Query::hasStatement() const { return !p->sql.empty(); }

/*!
 * Sets the parameters to \a args.
 *
 * \pre  \a args must be a well-formed JSON array and should not contain a null character.
 *
 * \note The bulk arguments are implicitly cleared.
 */
void Query::setArguments(const std::string &args) {
  p->args = args;
  p->bulkArgs.clear();
}

/*!
 * Returns the parameters.
 */
const std::string &Query::arguments() const { return p->args; }

/*!
 * Returns whether the query has parameters defined.
 */
bool Query::hasArguments() const { return !p->args.empty(); }

/*!
 * Sets the bulk arguments to \a bulkArgs.
 *
 * \pre  The elements of \a bulkArgs must be well-formed JSON arrays and should not contain a null
 *       character.
 *
 * \note The simple arguments are implicitly cleared.
 */
void Query::setBulkArguments(const std::vector<std::string> &bulkArgs) {
  p->bulkArgs = bulkArgs;
  p->args.clear();
}

/*!
 * Returns the bulk arguments.
 */
const std::vector<std::string> &Query::bulkArguments() const { return p->bulkArgs; }

/*!
 * Returns whether the query has bulk arguments defined.
 */
bool Query::hasBulkArguments() const { return !p->bulkArgs.empty(); }

}  // namespace CppCrate
