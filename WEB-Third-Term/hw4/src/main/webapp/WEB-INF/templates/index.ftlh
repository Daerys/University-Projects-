<#import "./common.ftlh" as c>

<@c.page>
    <#list posts?reverse as p>
        <@c.post p true/>
    </#list>
</@c.page>
