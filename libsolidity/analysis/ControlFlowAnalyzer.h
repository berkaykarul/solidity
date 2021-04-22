/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
// SPDX-License-Identifier: GPL-3.0

#pragma once

#include <libsolidity/analysis/ControlFlowGraph.h>
#include <set>

namespace solidity::frontend
{

class ControlFlowAnalyzer: private ASTConstVisitor
{
public:
	explicit ControlFlowAnalyzer(CFG const& _cfg, langutil::ErrorReporter& _errorReporter):
		m_cfg(_cfg), m_errorReporter(_errorReporter) {}

	bool analyze(ASTNode const& _astRoot);

	bool visit(FunctionDefinition const& _function) override;
	bool visit(ContractDefinition const& _contract) override;

private:
	void analyze(FunctionDefinition const& _function, ContractDefinition const* _contract = nullptr);
	/// Checks for uninitialized variable accesses in the control flow between @param _entry and @param _exit.
	void checkUninitializedAccess(CFGNode const* _entry, CFGNode const* _exit, bool _emptyBody, FunctionDefinition const& _function, ContractDefinition const* _contract);
	/// Checks for unreachable code, i.e. code ending in @param _exit, @param _revert or @param _transactionReturn
	/// that can not be reached from @param _entry.
	void checkUnreachable(CFGNode const* _entry, CFGNode const* _exit, CFGNode const* _revert, CFGNode const* _transactionReturn);

	bool checkForReverts(ContractDefinition const& _contract, FunctionCall const& _functionCall);

	CFG const& m_cfg;
	langutil::ErrorReporter& m_errorReporter;

	std::set<langutil::SourceLocation> m_previousUnreachable;
	std::set<VariableDeclaration const*> m_previousVariableWarnings;
	std::map<std::pair<ContractDefinition const*, FunctionDefinition const*>, FunctionFlow const*> m_functionFlows;
	enum RevertState { Reverting, NoRevert, Pending };
	std::map<std::pair<ContractDefinition const*, FunctionDefinition const*>, RevertState> m_functionReverts;
};

}
