/*
   This file is part of the clazy static checker.

  Copyright (C) 2017 Sergio Martins <smartins@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "ClangTidy.h"
#include "ClangTidyModule.h"
#include "ClangTidyModuleRegistry.h"
#include "checkmanager.h"
#include "ClazyContext.h"
#include "Clazy.h"
#include "ClazyAnchorHeader.h"

namespace clang {
namespace tidy {
namespace clazy {

//static ClazyASTConsumer *s_clazyASTConsumer = nullptr;

// Adaptor between clazy checks and clang-tidy checks
class ClazyClangTidyCheck : public ClangTidyCheck
{
public:
    ClazyClangTidyCheck(const RegisteredCheck &requestedClazyCheck, ClangTidyContext *tidyContext)
        : ClangTidyCheck(std::string("clazy-") + requestedClazyCheck.name, tidyContext)
        , m_clazyASTConsumer(new ClazyContext(tidyContext->getCompiler()))
    {
        CheckBase *clazyCheck = requestedClazyCheck.factory(m_clazyASTConsumer.context());
        m_clazyASTConsumer.addCheck(clazyCheck);
    }

    void registerMatchers(ast_matchers::MatchFinder *finder) override
    {
        finder->addMatcher(ast_matchers::translationUnitDecl().bind("tuForClazy"), this);
    }

    void check(const ast_matchers::MatchFinder::MatchResult &result) override
    {
        m_clazyASTConsumer.HandleTranslationUnit(m_clazyASTConsumer.context()->astContext);
    }

    ClazyASTConsumer m_clazyASTConsumer;
};

class ClazyModule : public ClangTidyModule
{
public:

    ClazyModule()
    {
        // Makes sure the linker doesn't ignore clazy, otherwise clazy's checks wouldn't be registed
        static int LLVM_ATTRIBUTE_UNUSED dummy = clazy_dummy();
    }

    void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override
    {
        RegisteredCheck::List checks = CheckManager::instance()->availableChecks(CheckLevel2);
        for (const RegisteredCheck &clazyCheck : checks) {
            CheckFactories.registerCheckFactory(std::string("clazy-") + clazyCheck.name,
            [clazyCheck](StringRef, ClangTidyContext *Context) {
                return new ClazyClangTidyCheck(clazyCheck, Context);
            });
        }
    }




};

// Register the ClazyModule using this statically initialized variable.
static ClangTidyModuleRegistry::Add<ClazyModule>
    X("clazy-module", "Adds clazy-related checks.");

} // namespace clazy

// This anchor is used to force the linker to link in the generated object file
// and thus register the ClazyModule.
volatile int ClazyModuleAnchorSource = 0;

}
}
