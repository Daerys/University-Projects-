<#-- @ftlvariable name="user_id" type="java.lang.Long" -->
<#-- @ftlvariable name="handle" type="java.lang.String" -->
<#import "./common.ftlh" as c>

<@c.page>
    <#if handle??>
        <#assign uh = c.findBy(users, "handle", handle)!/>
        <@c.userpage uh "handle" handle/>
    <#elseif user_id?? && user_id?is_number>
        <#assign ui = c.findBy(users, "id", user_id)!/>
        <@c.userpage ui "id" user_id/>
    <#else>
        <h1>No such user.</h1>
    </#if>
</@c.page>