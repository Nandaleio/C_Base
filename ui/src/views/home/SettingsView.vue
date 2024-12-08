<script setup lang="ts">

    import {
    Table,
    TableBody,
    TableCell,
    TableHead,
    TableHeader,
    TableRow,
    } from '@/components/ui/table'
    import {
    Dialog,
    DialogTrigger,
    } from '@/components/ui/dialog'
    import { Button } from '@/components/ui/button';
    import { Pencil, Trash2 } from 'lucide-vue-next'

    import AddAdmin from '@/components/AddAdmin.vue';
    import { cbFetch } from '@/services/api-service';
    import { onMounted, ref, useTemplateRef } from 'vue';

    const loading = ref(false);


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
    <div class="settings-container p-4">
        <h2 class="scroll-m-20 border-b pb-2 text-3xl font-semibold tracking-tight transition-colors first:mt-0">
            Settings
        </h2>
        <p class="leading-7 [&:not(:first-child)]:mb-6">Configure your app and the admins</p>

        <div class="actions">
            <Dialog>
                <DialogTrigger>
                    <Button variant="outline">
                        Add new Admin
                    </Button>
                </DialogTrigger>
                <AddAdmin ref="create-admin"></AddAdmin>
            </Dialog>
        </div>


        <Table v-if="cols && cols.length">
                <TableHeader>
                <TableRow>
                    <TableHead v-for="col of cols">
                    {{col}}
                    </TableHead>
                    <TableHead>
                        Actions
                    </TableHead>
                </TableRow>
                </TableHeader>
                <TableBody>
                <TableRow v-for="d of data">
                    <TableCell v-for="col of cols">
                    {{ d[col] }}
                    </TableCell>
                    <TableCell class="flex gap-1.5 justify-center">
                        <Button variant="secondary">
                            <Pencil />
                        </Button>
                        <Button variant="secondary">
                            <Trash2 />
                        </Button>
                    </TableCell>
                </TableRow>
                </TableBody>
            </Table>
    </div>
</template>

<style scoped>
.logout-btn {
    position: absolute;
    right: 1rem;
    top: 1rem;
    display: flex;
}
.settings-container {
    padding: 1rem;
    display: flex;
    flex-direction: column;
}
</style>