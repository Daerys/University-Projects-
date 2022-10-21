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
            {"href": "/index", "name": "Home"},
            {"href": "/misc/help", "name": "Help"},
            {"href": "/contests", "name": "Contests"}
            ]/>
            <ul>
                <#list items as item>
                    <li>
                        <a href="${item.href}">${item.name}</a>
                    </li>
                </#list>
            </ul>
        </nav>
    </header>
</#macro>

<#macro sidebar>
    <aside>
        <section>
            <div class="header">Information</div>
            <div class="body">Lorem ipsum dolor sit amet, consectetur adipisicing elit. Asperiores enim expedita facilis
                fugiat ipsum iste nobis reprehenderit tempore ut voluptatibus?
            </div>
            <div class="footer">
                <a href="#">View all</a>
            </div>
        </section>
        <section>
            <div class="header">Information</div>
            <div class="body">Lorem ipsum dolor sit amet, consectetur adipisicing elit. Asperiores enim expedita facilis
                fugiat ipsum iste nobis reprehenderit tempore ut voluptatibus?
            </div>
            <div class="footer">
                <a href="#">View all</a>
            </div>
        </section>
        <section>
            <div class="header">Information</div>
            <div class="body">Lorem ipsum dolor sit amet, consectetur adipisicing elit. Asperiores enim expedita facilis
                fugiat ipsum iste nobis reprehenderit tempore ut voluptatibus?
            </div>
            <div class="footer">
                <a href="#">View all</a>
            </div>
        </section>
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

<#macro userpage u key value>
<#-- @ftlvariable name="u" type="ru.itmo.web.hw4.model.User" -->
    <#assign p = findPrevious(users, key, value)!/>
    <#assign n = findNext(users, key, value)!/>
    <#if u?has_content>
    <div class="profileBox">
        <div class="body">
            <div class="info">
                <h1 class="name"><a href="/user?user_id=${u.id}">${u.name}</a></h1>
                <ul>
                    <li>${u.id}</li>
                    <li>${u.handle}</li>
                    <li>${u.name}</li>
                </ul>
            </div>
            <div class="photo"><img src="/img/icon.png" alt="Icon"></div>
        </div>
        <div class="footer">

            <#if !p?has_content || p.id == u.id>
                <a class="invalid" href="/user?user_id=-1">←</a>
            <#else>
                <a class="valid" href="/user?handle=${p.handle}">←</a>
            </#if>
            <#if n?has_content>
                <a class="valid" href="/user?handle=${n.handle}">→</a>
            <#else>
                <a class="invalid" href="/user?user_id=-1">→</a>
            </#if>
        </div>
    </div>
    <#else> <h1>No such user.</h1></#if>
</#macro>

