#pragma once
#include <drogon/HttpController.h>
using namespace drogon;
class Alter : public drogon::HttpController<Alter> {
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(Alter::AddData, "/data", Post, "AuthFilter"); // 推荐加上认证过滤器
  METHOD_ADD(Alter::AlterData, "/data/{id}", Put, "AuthFilter");
  METHOD_ADD(Alter::DeleteData, "/data/{id}", Delete, "AuthFilter");
  METHOD_LIST_END

  void AddData(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback);

  void AlterData(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback,
                 const std::string &id); // id会从路径 /data/{id} 自动传入

  void DeleteData(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  const std::string &id);
};

// const handleAddData = async () => {
//   try {
//     // axios.post(url, data)
//     // 第二个参数是请求体(request body)，axios 会自动将其序列化为 JSON
//     const response = await api.post('/data', {
//       key: newData.value.key,
//       // 假设后端需要一个JSON对象，我们在这里解析一下
//       value: JSON.parse(newData.value.value)
//     });
//     alert('Data added successfully!');
//     console.log(response.data);
//   } catch (error) {
//     alert('Failed to add data.');
//     console.error(error.response?.data || error.message);
//   }
// };

// /**
//  * 对应后端: AlterData(..., const std::string &id)
//  * HTTP请求: PUT /data/{id}
//  */
// const handleAlterData = async () => {
//   if (!updateId.value) {
//     alert('Please enter an ID to update.');
//     return;
//   }
//   try {
//     // 使用模板字符串构建带 ID 的 URL
//     // axios.put(url, data)
//     const response = await api.put(`/data/${updateId.value}`, {
//       key: updateData.value.key,
//       value: JSON.parse(updateData.value.value)
//     });
//     alert('Data altered successfully!');
//     console.log(response.data);
//   } catch (error) {
//     alert('Failed to alter data.');
//     console.error(error.response?.data || error.message);
//   }
// };

// /**
//  * 对应后端: DeleteData(..., const std::string &id)
//  * HTTP请求: DELETE /data/{id}
//  */
// const handleDeleteData = async () => {
//   if (!deleteId.value) {
//     alert('Please enter an ID to delete.');
//     return;
//   }
//   if (!confirm(`Are you sure you want to delete data with ID
//   ${deleteId.value}?`)) {
//     return;
//   }
//   try {
//     // DELETE 请求通常没有请求体
//     // axios.delete(url)
//     const response = await api.delete(`/data/${deleteId.value}`);
//     alert('Data deleted successfully!');
//     console.log(response.data);
//   } catch (error) {
//     alert('Failed to delete data.');
//     console.error(error.response?.data || error.message);
//   }
// };