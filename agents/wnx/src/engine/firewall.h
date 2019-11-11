// engine to install/remove firewall rule
// namespace cma::fw

#ifndef firewall_h__
#define firewall_h__

#pragma once

#include <netfw.h>

#include <functional>

#include "tools/_raii.h"

namespace cma::fw {

constexpr std::wstring_view kRuleGroup = L"CheckMK Firewall Control";
constexpr std::wstring_view kRuleDescription = L"Allow inbound network traffic";

bool CreateInboundRule(std::wstring_view rule_name, std::wstring_view app_name,
                       int port);

bool RemoveRule(std::wstring_view rule_name);

// mid-level API to be used with functor
// functor should find an appropriate rule anf return it to stop scanning
INetFwRule* ScanAllRules(std::function<INetFwRule*(INetFwRule*)> processor);

// Dump API, do not use it production
INetFwRule* DumpFWRulesInCollection(INetFwRule* fw_rule);
inline void DumpAllRules() { ScanAllRules(DumpFWRulesInCollection); }

INetFwRule* FindRule(std::wstring_view name);
INetFwRule* FindRule(std::wstring_view name, std::wstring_view app_name);

// proxy class to keep Windows Firewall API maximally isolated
class Policy {
public:
    Policy(const Policy& p) = delete;
    Policy& operator=(const Policy& p) = delete;
    Policy(const Policy&& p) = delete;
    Policy& operator=(Policy&& p) = delete;

    Policy();
    ~Policy();

    INetFwRules* getRules() { return rules_; }
    long getRulesCount();
    long getCurrentProfileTypes();

    IEnumVARIANT* getEnum();

private:
    INetFwPolicy2* policy_ = nullptr;
    INetFwRules* rules_ = nullptr;

#if defined(GTEST_INCLUDE_GTEST_GTEST_H_)
    friend class Firewall;
    FRIEND_TEST(Firewall, PolicyTest);
#endif
};

}  // namespace cma::fw

#endif  // firewall_h__
