<script setup lang="ts">
    import AddAdmin from '@/components/AddAdmin.vue';
import { cbFetch } from '@/services/api-service';
    import { onMounted, ref, useTemplateRef } from 'vue';

    const loading = ref(false);
    const createAdminOpen = useTemplateRef('create-admin');


    const cols = ref<string[]>([]);
    const data = ref<{ [key: string]: string }[]>([]);

    async function queryAdmins() {
        loading.value = true;
        const res = await cbFetch(`/api/admin/admins`)
        cols.value = res.columns;
        data.value = res.data;
        loading.value = false;
    }

    onMounted(async () => {
        await queryAdmins();
    })
</script>

<template>
    <div class="settings-container">
        <hgroup>
            <h2>Settings</h2>
            <p>Configure your app and the admins</p>
        </hgroup>

        <div class="actions">
            <button class="primary" @click="createAdminOpen?.open()">Add a new Admin</button>
            <AddAdmin ref="create-admin"></AddAdmin>
        </div>
        <table v-if="cols && cols.length">
                <thead>
                    <tr>
                    <th v-for="col of cols">
                        {{ col }}
                    </th>
                    </tr>
                </thead>
                <tbody>
                    <tr v-for="d of data">
                        <td v-for="col of cols">
                            {{ d[col] }}
                        </td>
                    </tr>
                </tbody>
            </table>
    </div>
</template>

<style scoped>
.settings-container {
    padding: 1rem;
    display: flex;
    flex-direction: column;
}
</style>