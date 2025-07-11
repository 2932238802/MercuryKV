
#include "KVStore/KvStore.h"
#include "KV_Tag/KvTagAssociation.h"
#include "Tags/Tags.h"
#include "type.hpp"
#include <cstdint>
#include <drogon/HttpResponse.h>
#include <drogon/orm/DbClient.h>
#include <drogon/utils/coroutine.h>
#include <json/value.h>
#include <memory>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <trantor/utils/Date.h>

namespace Service
{
using db_t = drogon::orm::DbClientPtr;
class AddDataServiceFactory;

// 删除之后 返回的内容
struct DeleteDataReturn
{
    int64_t code;
    std::string message;
};

class DeleteDataService
{
  public:
    drogon::orm::DbClientPtr db_client;
    using ptr = std::shared_ptr<DeleteDataService>;
    virtual drogon::Task<DeleteDataReturn> DeleteData(const std::string &kv_id);

  protected:
    DeleteDataService(db_t db_out) : db_client(db_out)
    {
    }
};

class DeleteDataServiceFactory
{
  public:
    static DeleteDataService::ptr MakeService(db_t db_out)
    {
        struct Enable : public DeleteDataService
        {
            Enable(db_t db_out) : DeleteDataService(db_out)
            {
            }
        };
        return std::make_shared<Enable>(db_out);
    }
};

} // namespace Service