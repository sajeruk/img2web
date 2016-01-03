# img2web
Micro-service of online image microblogs

img2web REST API:

INTERFACE
===============
<b>REQUEST</b>
<ul>
    <li>URL: /feed</li>
    <li>METHOD: GET</li>
    <li>PARAMS: page_number, login</li>
    <li>
    Примечание: page_number - некое число 0 .... N - пользователю будут показаны изображения в промежутке:
        (page_number * 10, page_number * 10 + 10) // 10 изображений на страницу</li>
</ul>
<b>RESPONSE</b>
<ul>
    <li>Content-Type: application/json</li>
    <li>Code: Success - 200 OK</li>
    <li>Body if success:
        {[{"login" : login1, image_binary_format" : image_binary_format1},
          {"login" : login2, image_binary_format" : image_binary_format2}... ]}</li>
</ul>
--------
<b>REQUEST</b>
<ul>
    <li>URL: /users  // возвращает логины всех, когда либо логинившихся пользователей</li>
    <li>METHOD: GET</li>

</ul>
<b>RESPONSE</b>
<ul>
    <li>Content-Type: application/json</li>
    <li>Code: Success - 200</li>
    <li>Body if success:
        {[{"login": usr_login}, ... ]}</li>
</ul>
--------
<b>REQUEST</b>
<ul>
    <li>URL: /subscribe // добавлять в ленту пользователя с логином login записи пользователя username</li>
    <li>METHOD: POST</li>
    <li>PARAMS: login, username</li> 
</ul>
<b>RESPONSE</b>
<ul>
    <li>Content-Type: application/json</li>
    <li>Code: Success - 200 FAIL - 434 // login or username does not exist </li>
</ul>
--------
<b>REQUEST</b>
<ul>
    <li>URL: /post</li>
    <li>METHOD: POST</li>
    <li>PARAMS: login, image_binary_format</li>
</ul>
<b>RESPONSE</b>
<ul>
    <li>Content-Type: application/json</li>
    <li>Code: Success - 200 FAIL - 434 Incorrect login</li>
</ul>
