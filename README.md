# img2web
Micro-service of online image microblogs

img2web REST API:

Login / Register
=================

<b>REQUEST</b>
<ul>
    <li>URL: /register</li>
    <li>METHOD: POST</li>
    <li>PARAMS: login, password</li>
</ul>
<b>RESPONSE</b>
    <li>Code: Success - 200 OK Fail - ??? LOGIN ALREADY USED</li>

<b>REQUEST</b>
    <li>URL: /login</li>
    <li>METHOD: POST</li>
    <li>PARAMS: login, password</li>
<b>RESPONSE</b>
    <li>Code: Success - 200 OK Fail - ??? WRONG LOGIN / PASSWORD</li>

<b>REQUEST</b>
    <li>URL: /sign_off</li>
    <li>METHOD: POST</li>
<b>RESPONSE</b>
    <li>Code: Success - 200 OK Fail - ??? NOT LOGGED IN</li>

INTERFACE
===============
<b>REQUEST</b>
    <li>URL: /feed</li>
    <li>METHOD: GET</li>
<b>RESPONSE</b>
    <li>Content-Type: application/json</li>
    <li>Code: Success - 200 OK Fail - ??? NOT_LOGGED_IN</li>
    <li>Body if success:
        {[{"image_name" : im_name, "url_to_img" : }, ... ]}</li>

<b>REQUEST</b>
    - <li>URL: /users
    - <li>METHOD: GET
<b>RESPONSE</b>
    - <li>Content-Type: application/json
    - <li>Code: Success - 200
    - <li>Body if success:
        {[{"user_login": usr_login}, ... ]}</li>

<b>REQUEST</b>
    - <li>URL: /subscribe</li>
    - <li>METHOD: POST</li>
    - <li>PARAMS: username</li>
<b>RES<PONSE</b>
    - <li>Content-Type: application/json</li>
    - <li>Code: Success - 200 FAIL - ??? NOT_LOGGED_IN, ??? - ALREADY_SUBSCRIBED</li>
