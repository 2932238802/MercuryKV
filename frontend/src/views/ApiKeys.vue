<script setup>
import { useRouter } from 'vue-router';
const router = useRouter();

const HandleReturn = () =>
{
    router.push({name:'Index'});
}

</script>

<template>
    <div class="page-container">
        <div class="content-card">
            <button class="btn-return" @click="HandleReturn" title="Go back">
                <i class="fas fa-arrow-left"></i>
            </button>

            <header class="card-header">
                <h1 class="title">API Keys</h1>
                <button class="btn btn-primary">
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
                        <tr>
                            <td>My Web App</td>
                            <td>
                                <div class="key-prefix"><code>sk_live_...a1b2</code></div>
                            </td>
                            <td><span class="status-badge active">Active</span></td>
                            <td>2024-07-09</td>
                            <td>
                                <button class="btn-icon" title="Revoke"><i class="fas fa-ban"></i></button>
                                <button class="btn-icon danger" title="Delete"><i class="fas fa-trash-alt"></i></button>
                            </td>
                        </tr>
                        <tr>
                            <td>Old Mobile App</td>
                            <td>
                                <div class="key-prefix"><code>sk_test_...c3d4</code></div>
                            </td>
                            <td><span class="status-badge revoked">Revoked</span></td>
                            <td>2023-01-15</td>
                            <td>
                                <button class="btn-icon danger" title="Delete"><i class="fas fa-trash-alt"></i></button>
                            </td>
                        </tr>
                    </tbody>
                </table>
            </div>
        </div>
    </div>
</template>

<style scoped>
@import url('../static/Common.css');

.key-prefix {
    display: inline-block;
    padding: 0.4rem 0.8rem;
    background-color: #f3f4f6;
    border-radius: 6px;
    font-family: 'SF Mono', 'Courier New', monospace;
    color: #4b5563;
}

.status-badge {
    display: inline-block;
    padding: 0.25rem 0.75rem;
    border-radius: 50px;
    font-size: 0.8rem;
    font-weight: 500;
}

.status-badge.active {
    background-color: #e5e7eb;
    color: #1f2937;
}

.status-badge.revoked {
    background-color: #f3f4f6;
    color: #9ca3af;
    text-decoration: line-through;
}
</style>
