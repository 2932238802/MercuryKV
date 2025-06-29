import axios from "axios";

const service = axios.create({
  baseURL: "/",
  timeout: 5000,
});

service.interceptors.request.use(
  (config) => {
    const token = localStorage.getItem("AuthToken");
    if (token) {
      // TODO : 后端看一眼 Authorization 是否正确
      config.headers["Authorization"] = "Bearer " + token;
    }
    return config;
  },
  (error) => {
    console.error("请求配置错误: ", error);
    return Promise.reject(error);
  }
);

service.interceptors.response.use(
  (response) => {
    const res = response.data;
    if (res.code !== 201 && res.code !== 200) {
      console.error("业务错误:", res.message);
      return Promise.reject(new Error(res.message || "Error"));
    } else {
      return res;
    }
  },
  (error) => {
    console.error("发生错误错误: " + error.data);
    return Promise.reject(error.data);
  }
);

export default service;
