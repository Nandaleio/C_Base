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
import { onMounted, ref } from 'vue';
import { Separator } from '@/components/ui/separator'
import type { Cols, Config } from '@/utils/types';
import Input from '@/components/ui/input/Input.vue';
import { Switch } from '@/components/ui/switch'

import {
  NumberField,
  NumberFieldContent,
  NumberFieldDecrement,
  NumberFieldIncrement,
  NumberFieldInput,
} from '@/components/ui/number-field'

const adminCols = ref<Cols[]>([]);
const adminData = ref<{ [key: string]: string }[]>([]);

const config = ref<Config[]>([]);


async function queryAdmins() {
    const res = await cbFetch(`/api/admin/admins`)
    adminCols.value = res.columns;
    adminData.value = res.data;
}

async function queryConfig() {
    const res = await cbFetch(`/api/admin/configs`)
    config.value = res.data;
}

onMounted(async () => {
    await queryAdmins();
    await queryConfig();
})
</script>

<template>
    <div class="settings-container p-4">
        <h2 class="scroll-m-20 border-b pb-2 text-3xl font-semibold tracking-tight transition-colors first:mt-0">
            Settings
        </h2>
        <p class="leading-7 [&:not(:first-child)]:mb-6">Configure your app and the admins</p>


        <Separator class="m-4" label="Configuration" />

        <Table v-if="config && config.length">
            <TableBody>
                <TableRow v-for="conf of config">
                    <TableCell>
                        {{ conf.name }}
                    </TableCell>
                    <TableCell>
                        <Switch v-if="conf.type === 'BOOLEAN'" :checked="conf.value == 'true'" @update:checked="(v) => conf.value = v+''"/>
                        <NumberField v-else-if="conf.type === 'NUMBER'" :model-value="+conf.value" @update:model-value="(v) => conf.value = v+''">
                            <NumberFieldContent>
                            <NumberFieldDecrement />
                            <NumberFieldInput />
                            <NumberFieldIncrement />
                            </NumberFieldContent>
                        </NumberField>
                        <Input v-else type="text" :model-value="conf.value" @update:model-value="(v) => conf.value = v+''"/>
                    </TableCell>
                </TableRow>
            </TableBody>
        </Table>
        <Button>
                        Save Change
                    </Button>

        <Separator class="m-4" label="Admin" />

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


        <Table v-if="adminCols && adminCols.length">
            <TableHeader>
                <TableRow>
                    <TableHead v-for="col of adminCols">
                        {{ col.name }}
                    </TableHead>
                    <TableHead>
                        Actions
                    </TableHead>
                </TableRow>
            </TableHeader>
            <TableBody>
                <TableRow v-for="d of adminData">
                    <TableCell v-for="col of adminCols">
                        {{ d[col.name] }}
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