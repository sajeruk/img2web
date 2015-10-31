# img2web
Micro-service of online image microblogs

img2web REST API:

Login / Register
=======

<b>REQUEST</b>
<ul>
    <li>URL: /register</li>
    <li>METHOD: POST</li>
    <li>PARAMS: login, password</li>
</ul>
<b>RESPONSE</b>
<ul>
    <li>Code: Success - 200 OK Fail - ??? LOGIN ALREADY USED</li>
</ul>
--------
<b>REQUEST</b>
<ul>
    <li>URL: /login</li>
    <li>METHOD: POST</li>
    <li>PARAMS: login, password</li>
</ul>
<b>RESPONSE</b>
<ul>
    <li>Code: Success - 200 OK Fail - ??? WRONG LOGIN / PASSWORD</li>
</ul>
--------
<b>REQUEST</b>
<ul>
    <li>URL: /sign_off</li>
    <li>METHOD: POST</li>
</ul>

<b>RESPONSE</b>
<ul>
    <li>Code: Success - 200 OK Fail - ??? NOT LOGGED IN</li>
</ul>
INTERFACE
===============
<b>REQUEST</b>
<ul>
    <li>URL: /feed</li>
    <li>METHOD: GET</li>
</ul>
<b>RESPONSE</b>
<ul>
    <li>Content-Type: application/json</li>
    <li>Code: Success - 200 OK Fail - ??? NOT_LOGGED_IN</li>
    <li>Body if success:
        {[{"image_name" : im_name, "url_to_img" : }, ... ]}</li>
</ul>
--------
<b>REQUEST</b>
<ul>
    <li>URL: /users</li>
    <li>METHOD: GET</li>
</ul>
<b>RESPONSE</b>
<ul>
    <li>Content-Type: application/json</li>
    <li>Code: Success - 200</li>
    <li>Body if success:
        {[{"user_login": usr_login}, ... ]}</li>
</ul>
--------
<b>REQUEST</b>
<ul>
    <li>URL: /subscribe</li>
    <li>METHOD: POST</li>
    <li>PARAMS: username</li>
</ul>
<b>RESPONSE</b>
<ul>
    <li>Content-Type: application/json</li>
    <li>Code: Success - 200 FAIL - ??? NOT_LOGGED_IN, ??? - ALREADY_SUBSCRIBED</li>
</ul>
