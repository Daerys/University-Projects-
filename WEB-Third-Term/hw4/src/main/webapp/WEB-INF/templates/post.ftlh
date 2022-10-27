<#-- @ftlvariable name="post_id" type="java.lang.Long" -->
<#import "./common.ftlh" as c>

<@c.page>
    <#if post_id?? && post_id?is_number>
        <#assign p = c.findBy(posts, "id", post_id)!/>
        <@c.post p false/>
    <#else>
        <h1>No such post.</h1>
    </#if>
</@c.page>