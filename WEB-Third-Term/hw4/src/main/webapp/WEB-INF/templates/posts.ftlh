<#-- @ftlvariable name="user_id" type="java.lang.Long" -->

<#import "./common.ftlh" as c>
<@c.page>
    <#if user_id?? && user_id?is_number>
        <#list posts as p>
        <#if p.user_id == user_id>
            <@c.post p false/>
        </#if>
    </#list>
    <#else>
    </#if>
</@c.page>