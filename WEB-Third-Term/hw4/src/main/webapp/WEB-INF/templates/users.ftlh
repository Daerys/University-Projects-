<#import "./common.ftlh" as c>

<@c.page>
    <div class="datatable">
        <div class="caption">Submissions</div>
        <table>
            <thead>
            <tr>
                <th>Id</th>
                <th>User</th>
            </tr>
            </thead>
            <tbody>

        <#list users as u>
            <tr>
                <td>${u.id}</td>
                <td><a><@c.userlink u/></a></td>
            </tr>
        </#list>
            </tbody>
        </table>
    </div>
</@c.page>