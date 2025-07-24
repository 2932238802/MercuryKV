<script setup>
import { ref } from 'vue';
import { useRouter } from 'vue-router';

const router = useRouter();

const show_addkeyform = ref(false); 
const newkeylabel = ref('');
const generatedkey = ref(null); 
const ApiKeys = ref([
]);

// 返回主页
const HandleReturn = () => {
    router.push({ name: 'Index' });
};

// 点击 "Generate New Key" 按钮时触发
const ShowGenerateForm = () => {
    newkeylabel.value = '';
    generatedkey.value = null; // 这个随机生成的 key
    show_addkeyform.value = true;
};

const HideGenerateForm = () => {
    show_addkeyform.value = false;
};

const HandleSubmitNewKey = () => {
    if (!newkeylabel.value.trim()) {
        alert("Please provide a name for your API key.");
        return;
    }

    const fullKey = `mercury${GenerateRandomString(48)}`;
    const newKeyObject = {
        id: `uuid-${Date.now()}`,
        label: newkeylabel.value,
        keyPrefix: `${fullKey.substring(0, 10)}...${fullKey.slice(-4)}`,
        status: 'Active',
        created: new Date().toISOString().split('T')[0], // 格式化为 YYYY-MM-DD
    };

    generatedkey.value = fullKey;
    ApiKeys.value.unshift(newKeyObject);
};

// 复制 Key 到剪贴板
const CopyToClipboard = async () => {
    if (!generatedkey.value) return;
    try {
        await navigator.clipboard.writeText(generatedkey.value);
        alert("API Key copied to clipboard!");
    } catch (err) {
        console.error('Failed to copy: ', err);
        alert("Failed to copy key. Please copy it manually.");
    }
};

const GenerateRandomString = (length) => {
    const chars = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
    let result = '';
    for (let i = 0; i < length; i++) {
        result += chars.charAt(Math.floor(Math.random() * chars.length));
    }
    return result;
};


const RevokeKey = (keyId) => {
    const keyToRevoke = ApiKeys.value.find(k => k.id === keyId);
    if (confirm(`你确定暂时注销这个"${keyToRevoke.label}"?`)) {
        keyToRevoke.status = 'Revoked';
    }
};

const DeleteKey = (keyId) => {
    const keyIndex = ApiKeys.value.findIndex(k => k.id === keyId); // 找到对应所以
    if (keyIndex === -1) return;
    const keytodelete = ApiKeys.value[keyIndex];
    if (confirm(`你确定删除"${keytodelete.label}"? 这行为不能撤回`)) {
        ApiKeys.value.splice(keyIndex, 1); // splice 左边是删除位置 右边是删除个数
    }
};

</script>

<template>
    <div class="page-container">
        <div v-if="show_addkeyform" class="add-key-container">
            <div class="content-card add-key-card">
                <form v-if="!generatedkey" @submit.prevent="HandleSubmitNewKey">
                    <header class="card-header">
                        <h1 class="title">Generate New API Key</h1>
                    </header>
                    <div class="form-group">
                        <label for="key-label">Name</label>
                        <input id="key-label" type="text" v-model="newkeylabel" placeholder="e.g., My Production Server" required>
                        <p class="form-hint">Give your key a descriptive name to remember its purpose.</p>
                    </div>
                    <div class="form-actions">
                        <button type="button" class="btn btn-secondary" @click="HideGenerateForm">Cancel</button>
                        <button type="submit" class="btn btn-primary">Generate Key</button>
                    </div>
                </form>

                <div v-else class="key-display-area">
                     <header class="card-header">
                        <h1 class="title">API Key Generated</h1>
                    </header>
                    <div class="alert-warning">
                        <i class="fas fa-exclamation-triangle"></i>
                        <span>Please save this secret key somewhere safe. You will not be able to view it again.</span>
                    </div>
                    <div class="generated-key-wrapper">
                        <code>{{ generatedkey }}</code>
                        <button class="btn-icon" @click="CopyToClipboard" title="Copy to clipboard">
                            <i class="fas fa-copy"></i>
                        </button>
                    </div>
                    <div class="form-actions">
                         <button type="button" class="btn btn-primary" @click="HideGenerateForm">Done</button>
                    </div>
                </div>
            </div>
        </div>

        <div v-else class="content-card">
            <button class="btn-return" @click="HandleReturn" title="Go back">
                <i class="fas fa-arrow-left"></i>
            </button>

            <header class="card-header">
                <h1 class="title">API Keys</h1>
                <button class="btn btn-primary" @click="ShowGenerateForm">
                    <i class="fas fa-plus"></i> Generate New Key
                </button>
            </header>

            <div class="table-wrapper">
                <table class="data-table">
                    <thead>
                        <tr>
                            <th>LABEL</th>
                            <th>KEY PREFIX</th>
                            <th>STATUS</th>
                            <th>CREATED</th>
                            <th>ACTIONS</th>
                        </tr>
                    </thead>
                    <tbody>
                        <tr v-for="key in ApiKeys" :key="key.id">
                            <td>{{ key.label }}</td>
                            <td>
                                <div class="key-prefix"><code>{{ key.keyPrefix }}</code></div>
                            </td>
                            <td>
                                <span class="status-badge" :class="{
                                    'active': key.status === 'Active',
                                    'revoked': key.status === 'Revoked'
                                }">
                                    {{ key.status }}
                                </span>
                            </td>
                            <td>{{ key.created }}</td>
                            <td>
                                <button v-if="key.status === 'Active'" class="btn-icon" title="Revoke" @click="RevokeKey(key.id)">
                                    <i class="fas fa-ban"></i>
                                </button>
                                <button class="btn-icon danger" title="Delete" @click="DeleteKey(key.id)">
                                    <i class="fas fa-trash-alt"></i>
                                </button>
                            </td>
                        </tr>
                        <tr v-if="ApiKeys.length === 0">
                            <td colspan="5" class="empty-state">
                                You haven't generated any API keys yet.
                            </td>
                        </tr>
                    </tbody>
                </table>
            </div>
        </div>
    </div>
</template>

<style scoped>
@import url('@/static/Common.css');
</style>
