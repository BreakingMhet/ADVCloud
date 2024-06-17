# Service Principal Abuse

## Description

In this scenario, an attacker exploits a misconfigured service principal to perform a privilege escalation attack. The attacker initially compromises an Application Administrator user account, which has full access to enterprise applications, including service principals. By identifying a service principal with an administrative role, the attacker leverages its privileges to generate new credentials for the service principal. Subsequently, the attacker uses these new credentials to log in and reassign the same administrative role to the compromised Application Administrator account.

## Requirements

- Application Administraor user;
- Enterprise Application (Service Principal) with Privileged Role Administrator or Global Admin.

## Reference

- https://posts.specterops.io/azure-privilege-escalation-via-service-principal-abuse-210ae2be2a5;