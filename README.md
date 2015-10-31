# img2web
Micro-service of online image microblogs

img2web REST API:

Login / Register
=================

<b>REQUEST</b>
    - URL: /register
    - METHOD: POST
    - PARAMS: login, password
<b>RESPONSE</b>
    - Code: Success - 200 OK Fail - ??? LOGIN ALREADY USED

<b>REQUEST</b>
    - URL: /login
    - METHOD: POST
    - PARAMS: login, password
<b>RESPONSE</b>
    - Code: Success - 200 OK Fail - ??? WRONG LOGIN / PASSWORD

<b>REQUEST</b>
    - URL: /sign_off
    - METHOD: POST
<b>RESPONSE</b>
    - Code: Success - 200 OK Fail - ??? NOT LOGGED IN

INTERFACE
===============
<b>REQUEST</b>
    - URL: /feed
    - METHOD: GET
<b>RESPONSE</b>
    - Content-Type: application/json
    - Code: Success - 200 OK Fail - ??? NOT_LOGGED_IN
    - Body if success:
        {[{"image_name" : im_name, "url_to_img" : }, ... ]}

<b>REQUEST</b>
    - URL: /users
    - METHOD: GET
<b>RESPONSE</b>
    - Content-Type: application/json
    - Code: Success - 200
    - Body if success:
        {[{"user_login": usr_login}, ... ]}

<b>REQUEST</b>
    - URL: /subscribe
    - METHOD: POST
    - PARAMS: username
<b>RESPONSE</b>
    - Content-Type: application/json
    - Code: Success - 200 FAIL - ??? NOT_LOGGED_IN, ??? - ALREADY_SUBSCRIBED
