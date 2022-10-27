<#macro page>
    <!DOCTYPE html>
    <html lang="ru">
    <head>
        <meta charset="UTF-8">
        <title>Codeforces</title>
        <link rel="stylesheet" href="/css/normalize.css">
        <link rel="stylesheet" href="/css/style.css">
        <link rel="icon" href="/favicon.ico">
    </head>
    <body>
    <@header/>
    <div class="middle">
        <@sidebar/>
        <main>
            <#nested/>
        </main>
    </div>
    <@footer/>
    </body>
    </html>
</#macro>

<#macro header>
    <header>
        <a href="#" class="logo"><img alt="Codeforces" title="Codeforces"
                                      src="/img/codeforces-logo-with-telegram.png"></a>
        <div class="languages">
            <a href="#"><img alt="In English" title="In English"
                             src="/img/gb.png"></a>
            <a href="#"><img alt="In Russian" title="In Russian"
                             src="/img/ru.png"></a>
        </div>
        <div class="enter-or-register-box">
            <#if u??>
                <@userlink u/>
                |
                <a href="/logout">Logout</a>
            <#else>
                <a href="/enter">Enter</a>
                |
                <a href="/register">Register</a>
            </#if>
        </div>
        <nav>
            <#assign items=[
            {"href": "/index", "name": "Home", "highlight": ["/index", "/"]},
            {"href": "/misc/help", "name": "Help", "highlight": ["/misc/help"]},
            {"href": "/contests", "name": "Contests", "highlight": ["/contests"]},
            {"href": "/users", "name": "Users", "highlight": ["/users"]}
            ]/>
            <ul>
                <#list items as item>
                    <li>
                        <#if item.highlight?seq_contains(uri)>
                            <a class="highlight" href="${item.href}">${item.name}</a>
                        <#else>
                            <a href="${item.href}">${item.name}</a>
                        </#if>
                    </li>
                </#list>
            </ul>
        </nav>
    </header>
</#macro>

<#macro sidebar>
    <aside>
        <#list posts as p>
            <section>
                <div class="header">${p.title}</div>
                <@postBody p true/>
                <div class="footer">
                    <a href="/post?post_id=${p.id}">View all</a>
                </div>
            </section>
        </#list>
    </aside>
</#macro>

<#macro footer>
    <footer>
        2010-2021 &copy; by Mike Mirzayanov
    </footer>
</#macro>

<#macro userlink user>
<#-- @ftlvariable name="u" type="ru.itmo.web.hw4.model.User" -->
    <a href="/user?handle=${user.handle}">${user.name}</a>
</#macro>

<#macro userpage u key value>
<#-- @ftlvariable name="u" type="ru.itmo.web.hw4.model.User" -->
    <#assign pr = findPrevious(users, key, value)!/>
    <#assign nx = findNext(users, key, value)!/>
    <#if u?has_content>
        <div class="profileBox">
            <div class="body">
                <div class="info">
                    <h1 class="name"><a href="/user?user_id=${u.id}">${u.name}</a></h1>
                    <ul>
                        <#assign cnt = countPosts(posts, u.id)>
                        <li>user id: ${u.id}</li>
                        <li>user handel: ${u.handle}</li>
                        <li>user name: ${u.name}</li>
                        <li>posts count: <a href="/posts?user_id=#{u.id}">${cnt}</a></li>
                    </ul>
                </div>
                <div class="photo"><img src="/img/icon.png" alt="Icon"></div>
            </div>
            <div class="footer">

                <#if !pr?has_content || pr.id == u.id>
                    <a class="invalid">←</a>
                <#else>
                    <a class="valid" href="/user?handle=${pr.handle}">←</a>
                </#if>
                <#if nx?has_content>
                    <a class="valid" href="/user?handle=${nx.handle}">→</a>
                <#else>
                    <a class="invalid">→</a>
                </#if>
            </div>
        </div>
    <#else> <h1>No such user.</h1></#if>
</#macro>


<#macro postBody p isShort>
    <div class="body">
        <#assign text = cutText(p.text, isShort)>
        ${text}
    </div>
</#macro>

<#macro post p isShort>
    <article>
        <#assign us = findBy(users, "id", p.user_id)>
        <div class="title">${p.title}</div>
        <div class="information">By <@userlink us /></div>
        <@postBody p isShort/>
        <div class="footer">
            <div class="left">
                <img src="img/voteup.png" title="Vote Up" alt="Vote Up"/>
                <span class="positive-score">+1</span>
                <img src="img/votedown.png" title="Vote Down" alt="Vote Down"/>
            </div>
            <div class="right">
                <img src="img/date_16x16.png" title="Publish Time" alt="Publish Time"/>
                2 days ago
                <img src="img/comments_16x16.png" title="Comments" alt="Comments"/>
                <a href="#">68</a>
            </div>
        </div>
    </article>
</#macro>

<#function cutText text isShort>
    <#if isShort && text?length gt 350>
        <#return text?substring(0,349) + "..."/>
    </#if>
    <#return text>
</#function>

<#function findBy items key value>
    <#list items as item>
        <#if item[key] == value>
            <#return item/>
        </#if>
    </#list>
</#function>

<#function findPrevious items key value>

    <#assign a = items[0]>
    <#list items as item>
        <#if item[key] == value>
            <#return a/>
        </#if>
        <#assign a = item>
    </#list>
</#function>

<#function findNext items key value>
    <#assign foo = false>
    <#list items as item>
        <#if foo>
            <#return item/>
        </#if>
        <#if item[key] == value>
            <#assign foo = true>
        </#if>
    </#list>
</#function>

<#function countPosts items uid>
    <#assign cnt = 0>
    <#list items as item>
        <#if item.user_id == uid>
            <#assign cnt = cnt + 1>
        </#if>
    </#list>
    <#return cnt/>
</#function>
