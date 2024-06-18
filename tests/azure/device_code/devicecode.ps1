# Prepare the body for the request

$body=@{
	"client_id" = "d3590ed6-52b3-4102-aeff-aad2292ab01c"
	"resource" =  "https://graph.microsoft.com/"
}

# Invoke the request to get device and user codes

$auth_response = Invoke-RestMethod -UseBasicParsing -Method Post -Uri "https://login.microsoftonline.com/common/oauth2/devicecode?api-version=1.0" -Body $body
$auth_response

# After completing the autentication press a key to request the access token 

Write-Host "Press any key after the authentication is completed to continue..."
Read-Host -Prompt ">"
Write-Host "You pressed a key. The script will now proceed..."

$body=@{
	"client_id" =  "d3590ed6-52b3-4102-aeff-aad2292ab01c"
	"grant_type" = "urn:ietf:params:oauth:grant-type:device_code"
	"code" =       $auth_response.device_code
	"resource" =   "https://graph.windows.net/user.readwrite.all"
}

$response = Invoke-RestMethod -UseBasicParsing -Method Post -Uri "https://login.microsoftonline.com/Common/oauth2/token?api-version=1.0 " -Body $body -ErrorAction SilentlyContinue
$access_token = $response.access_token

Write-Output "`n$access_token`n"

Write-Host "Press any key to login to Microsoft Graph using the Access Token..."
Read-Host -Prompt ">"
Write-Host "You pressed a key. The script will now proceed..."

$sec_token = ConvertTo-SecureString $access_token -AsPlainText -Force
Connect-MgGraph -AccessToken $sec_token
