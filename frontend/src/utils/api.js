import service from "./request.js";

const API_PATH = {
  TOKEN_CHECK_PATH: "/Login/checktoken",
  VERUFY_AND_MODIFY_PATH: "/EmailVerify/verifyandmodify",
  SENDCODE_PATH: "/EmailVerify/sendemail",
  LOGIN_PATH: "/Login/login",
  REGISTER_PATH: "/Register/register",
  MODIFY_PASSWORD_PATH:"/ModifyPassword/modifypd", // 修改密码
};

export const LOGIN = async (logininfo) => {
  console.log("LOGIN:",logininfo);
  const response = await service.post(API_PATH.LOGIN_PATH, logininfo);
  console.log(response);
  return response;
};

export const REGISTER = async (registerinfo) => {
  console.log("registerinfo:",registerinfo);
  const response = await service.post(API_PATH.REGISTER_PATH, registerinfo);
  console.log(response);
  return response;
};

export const SENDCODE = async (email_value) => {
  console.log("email_value:",email_value);

  const response = await service.get(API_PATH.SENDCODE_PATH, {
    params: { "email": email_value },
  });
  console.log(response);
  return response;
};

export const SEND_EMAIL_FORPROFILE = async (info) => {
  console.log("info:",info);
  const response = await service.post(API_PATH.VERUFY_AND_MODIFY_PATH, info);
  console.log(response);
  return response;
};

export const MODIFY_PASSWORD = async(info) =>{
  console.log("info",info)
  const response  = await service.post(API_PATH.MODIFY_PASSWORD_PATH,info);
  console.log(response);
  return response;
}

export const TOKEN_CHECK = async() =>{
  const response  = await service.get(API_PATH.TOKEN_CHECK_PATH);
  console.log(response);
  return response;
}